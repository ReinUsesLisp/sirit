/* Cooperative-matrix shader generation tests.
 *
 * Build a compute kernel that:
 *   1. declares OpTypeCooperativeMatrixKHR (component=float32, scope=Subgroup,
 *      rows=cols=16) for MatrixA / MatrixB / MatrixAccumulator;
 *   2. obtains zero-valued matrices via OpConstantNull;
 *   3. multiplies via OpCooperativeMatrixMulAddKHR.
 *
 * The kernel does not touch global memory -- the goal is to verify sirit
 * generates a structurally valid module that spirv-val accepts under the
 * SPV_KHR_cooperative_matrix capability gates. We also disassemble and look
 * for the expected opcodes / type id reuse.
 */

#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <sirit/sirit.h>

#include "test_helpers.h"

using namespace sirit_tests;

namespace {

constexpr std::uint32_t SpvVersion16 = 0x00010600;  // SPIR-V 1.6 (matches Vulkan 1.3)

// Build the module described in this file's header. Returns the assembled
// SPIR-V words plus the result-type Id so the caller can spot-check it in the
// disassembly.
std::vector<std::uint32_t> BuildCoopMatModule() {
    Sirit::Module m{SpvVersion16};

    m.AddCapability(spv::Capability::Shader);
    m.AddCapability(spv::Capability::VulkanMemoryModel);
    m.AddCapability(spv::Capability::CooperativeMatrixKHR);
    m.AddExtension("SPV_KHR_cooperative_matrix");
    m.AddExtension("SPV_KHR_vulkan_memory_model");
    m.SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::Vulkan);

    const auto t_void = m.TypeVoid();
    const auto t_uint = m.TypeInt(32, false);
    const auto t_float = m.TypeFloat(32);

    // Cooperative-matrix dimension / scope / use arguments must be specialization
    // ids referring to OpConstants. Subgroup scope value = 3.
    const auto c_scope_subgroup = m.Constant(t_uint, static_cast<std::uint32_t>(spv::Scope::Subgroup));
    const auto c_dim_16 = m.Constant(t_uint, std::uint32_t{16});
    const auto c_use_a = m.Constant(t_uint, static_cast<std::uint32_t>(
                                             spv::CooperativeMatrixUse::MatrixAKHR));
    const auto c_use_b = m.Constant(t_uint, static_cast<std::uint32_t>(
                                             spv::CooperativeMatrixUse::MatrixBKHR));
    const auto c_use_acc = m.Constant(t_uint, static_cast<std::uint32_t>(
                                              spv::CooperativeMatrixUse::MatrixAccumulatorKHR));

    const auto t_mat_a = m.TypeCooperativeMatrixKHR(t_float, c_scope_subgroup, c_dim_16,
                                                    c_dim_16, c_use_a);
    const auto t_mat_b = m.TypeCooperativeMatrixKHR(t_float, c_scope_subgroup, c_dim_16,
                                                    c_dim_16, c_use_b);
    const auto t_mat_c = m.TypeCooperativeMatrixKHR(t_float, c_scope_subgroup, c_dim_16,
                                                    c_dim_16, c_use_acc);

    // Type-dedup spot check before any function body lands.
    const auto t_mat_a_again = m.TypeCooperativeMatrixKHR(t_float, c_scope_subgroup, c_dim_16,
                                                          c_dim_16, c_use_a);
    CHECK(t_mat_a.value == t_mat_a_again.value);

    const auto t_func = m.TypeFunction(t_void);
    const auto fn = m.OpFunction(t_void, spv::FunctionControlMask::MaskNone, t_func);
    m.Name(fn, "main");
    m.AddLabel();

    const auto null_a = m.ConstantNull(t_mat_a);
    const auto null_b = m.ConstantNull(t_mat_b);
    const auto null_c = m.ConstantNull(t_mat_c);

    const auto product = m.OpCooperativeMatrixMulAddKHR(t_mat_c, null_a, null_b, null_c,
                                                        std::nullopt);
    (void)product;

    m.OpReturn();
    m.OpFunctionEnd();

    m.AddEntryPoint(spv::ExecutionModel::GLCompute, fn, "main");
    m.AddExecutionMode(fn, spv::ExecutionMode::LocalSize, std::uint32_t{32}, std::uint32_t{1},
                       std::uint32_t{1});
    return m.Assemble();
}

void test_coopmat_module_validates() {
    const auto words = BuildCoopMatModule();
    const auto result = Validate(words, SPV_ENV_VULKAN_1_3);
    if (!result.ok) {
        std::fputs(result.messages.c_str(), stderr);
    }
    CHECK(result.ok);
}

void test_coopmat_disassembly_contains_expected_ops() {
    const auto words = BuildCoopMatModule();
    const std::string text = Disassemble(words, SPV_ENV_VULKAN_1_3);
    CHECK(text.find("OpTypeCooperativeMatrixKHR") != std::string::npos);
    CHECK(text.find("OpCooperativeMatrixMulAddKHR") != std::string::npos);
    CHECK(text.find("CooperativeMatrixKHR") != std::string::npos);  // capability
    CHECK(text.find("\"SPV_KHR_cooperative_matrix\"") != std::string::npos);
}

}  // namespace

namespace sirit_tests {

void RegisterCooperativeMatrixTests() {
    RUN_TEST(test_coopmat_module_validates);
    RUN_TEST(test_coopmat_disassembly_contains_expected_ops);
}

}  // namespace sirit_tests
