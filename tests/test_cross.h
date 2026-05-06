/* SPIRV-Cross interface. Kept in its own header (separate from test_helpers.h)
 * because <spirv.hpp> (the unscoped enum form bundled with SPIRV-Cross) and
 * <spirv/unified1/spirv.hpp11> (the scoped enum form sirit's API uses) share
 * the same include guard `spirv_HPP`. Whichever one is #included first wins,
 * leaving the other's symbols undefined and breaking the build.
 *
 * test_cross.cpp must NOT include test_helpers.h or any sirit header to keep
 * spirv.hpp11 out of its translation unit.
 */

#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace sirit_tests {

struct CrossCompileResult {
    bool ok = false;
    std::string source;  // GLSL source on success
    std::string error;   // exception message on failure
};

/// Run SPIRV-Cross's GLSL backend against `words`. Used as a sanity check that
/// the produced module is structurally consumable by an external translator.
CrossCompileResult CrossToGlsl(std::span<const std::uint32_t> words,
                               std::uint32_t glsl_version = 460,
                               bool vulkan_semantics = true);

}  // namespace sirit_tests
