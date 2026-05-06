/* Implementation of CrossToGlsl. Isolated to its own TU so the
 * unscoped-enum spirv.hpp pulled in by SPIRV-Cross doesn't conflict with
 * sirit's spirv.hpp11 (both share the include guard `spirv_HPP`). This file
 * must not include test_helpers.h or any sirit header.
 */

#include "test_cross.h"

#include <exception>
#include <utility>
#include <vector>

#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace sirit_tests {

CrossCompileResult CrossToGlsl(std::span<const std::uint32_t> words,
                               std::uint32_t glsl_version,
                               bool vulkan_semantics) {
    CrossCompileResult result;
    try {
        std::vector<std::uint32_t> copy(words.begin(), words.end());
        spirv_cross::CompilerGLSL compiler(std::move(copy));
        spirv_cross::CompilerGLSL::Options opts;
        opts.version = glsl_version;
        opts.vulkan_semantics = vulkan_semantics;
        opts.es = false;
        compiler.set_common_options(opts);
        result.source = compiler.compile();
        result.ok = true;
    } catch (const std::exception& e) {
        result.ok = false;
        result.error = e.what();
    } catch (...) {
        result.ok = false;
        result.error = "<unknown SPIRV-Cross exception>";
    }
    return result;
}

}  // namespace sirit_tests
