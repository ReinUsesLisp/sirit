/* Shared infrastructure for sirit_tests.
 *
 * - Test-runner macros (CHECK / RUN_TEST) and the global counters they feed.
 * - SPIR-V validator + disassembler wrappers (SPIRV-Tools).
 * - SPIR-V -> GLSL round-trip helper (SPIRV-Cross).
 *
 * Every test source file includes this header. Globals are defined in
 * test_helpers.cpp.
 */

#pragma once

#include <cstdint>
#include <cstdio>
#include <map>
#include <span>
#include <string>
#include <vector>

#include <spirv-tools/libspirv.h>
#include <spirv/unified1/spirv.hpp11>

namespace sirit_tests {

// === Test-runner globals (defined in test_helpers.cpp) =====================

extern int g_total;
extern int g_failures;
extern const char* g_current_test;

#define CHECK(cond)                                                                                \
    do {                                                                                           \
        ++::sirit_tests::g_total;                                                                  \
        if (!(cond)) {                                                                             \
            ++::sirit_tests::g_failures;                                                           \
            std::fprintf(stderr, "  FAIL [%s] %s:%d: %s\n", ::sirit_tests::g_current_test,         \
                         __FILE__, __LINE__, #cond);                                               \
        }                                                                                          \
    } while (0)

#define RUN_TEST(fn)                                                                               \
    do {                                                                                           \
        ::sirit_tests::g_current_test = #fn;                                                       \
        const int before = ::sirit_tests::g_failures;                                              \
        fn();                                                                                      \
        std::fprintf(stderr, "%-44s %s\n", #fn,                                                    \
                     ::sirit_tests::g_failures == before ? "ok" : "FAILED");                       \
    } while (0)


// === SPIR-V validation and disassembly (SPIRV-Tools) =======================

struct ValidationResult {
    bool ok = false;
    std::string messages;  // diagnostic stream from spirv-val (empty on success)
};

/// Validate `words` for the given target environment. Returns ok=true if the
/// validator accepts the module. On failure `messages` carries spirv-val's
/// diagnostic text plus the disassembled module so a `CHECK(result.ok)` failure
/// is debuggable from the test log alone.
ValidationResult Validate(std::span<const std::uint32_t> words,
                          spv_target_env env = SPV_ENV_VULKAN_1_3);

/// Disassemble `words` to human-readable SPIR-V text. Empty on parse failure.
std::string Disassemble(std::span<const std::uint32_t> words,
                        spv_target_env env = SPV_ENV_VULKAN_1_3);


// SPIRV-Cross interface (CrossToGlsl) lives in test_cross.h -- its bundled
// spirv.hpp shares an include guard with sirit's spirv.hpp11 and they cannot
// both appear in the same translation unit.


// === SPIR-V text -> binary (SPIRV-Tools assembler) =========================

struct AssembleResult {
    bool ok = false;
    std::vector<std::uint32_t> words;
    std::string error;
};

/// Run SPIRV-Tools' assembler on `text`. Used by the shader-replication tests
/// to obtain a canonical reference binary from a textual SPIR-V "spec".
AssembleResult AssembleText(const std::string& text,
                            spv_target_env env = SPV_ENV_VULKAN_1_3);


// === Structural module comparison ==========================================
//
// The replication tests don't aim for byte-equal binaries: spirv-as and sirit
// allocate result-ids in different orders. Instead they assemble both via
// each path and compare a coarse-grained structural summary so equivalent
// modules compare equal.

struct EntryPointSummary {
    spv::ExecutionModel model{};
    std::string name;
    std::size_t interface_count{};

    bool operator==(const EntryPointSummary&) const = default;
};

struct ModuleSummary {
    std::vector<spv::Capability> capabilities;          // sorted, deduped
    std::vector<std::string> extensions;                 // sorted, deduped
    spv::AddressingModel addressing_model{};
    spv::MemoryModel memory_model{};
    std::vector<EntryPointSummary> entry_points;         // sorted by (model, name)
    std::map<spv::Op, std::size_t> opcode_counts;        // every emitted opcode

    bool operator==(const ModuleSummary&) const = default;
};

ModuleSummary Summarize(std::span<const std::uint32_t> words);

/// Pretty-print the diff between two summaries (for use in test failure
/// messages). Returns an empty string when the two compare equal.
std::string SummaryDiff(const ModuleSummary& expected, const ModuleSummary& actual);

}  // namespace sirit_tests
