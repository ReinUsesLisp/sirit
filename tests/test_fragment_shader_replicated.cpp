/* Fragment-shader replication test.
 *
 * Spec: a fragment shader that writes a constant red color (1, 0, 0, 1) to
 * output Location 0. Demonstrates the OriginUpperLeft execution mode, an
 * Output v4float variable, and OpConstantComposite.
 */

#include <cstdint>
#include <string>
#include <vector>

#include <sirit/sirit.h>

#include "test_helpers.h"

using namespace sirit_tests;

namespace {

constexpr std::uint32_t SpvVersion10 = 0x00010000;

const char* kFragmentSpvText = R"(
               OpCapability Shader
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %main "main" %out_color
               OpExecutionMode %main OriginUpperLeft
               OpDecorate %out_color Location 0
       %void = OpTypeVoid
         %fn = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v4float = OpTypeVector %float 4
%_ptr_Output_v4float = OpTypePointer Output %v4float
  %out_color = OpVariable %_ptr_Output_v4float Output
       %f0_0 = OpConstant %float 0
       %f1_0 = OpConstant %float 1
      %color = OpConstantComposite %v4float %f1_0 %f0_0 %f0_0 %f1_0
       %main = OpFunction %void None %fn
      %entry = OpLabel
                OpStore %out_color %color
                OpReturn
                OpFunctionEnd
)";

std::vector<std::uint32_t> BuildFragmentShaderWithSirit() {
    Sirit::Module m{SpvVersion10};

    m.AddCapability(spv::Capability::Shader);
    m.SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::GLSL450);

    const auto t_void = m.TypeVoid();
    const auto t_fn = m.TypeFunction(t_void);
    const auto t_float = m.TypeFloat(32);
    const auto t_v4float = m.TypeVector(t_float, 4);
    const auto t_ptr_out_v4 = m.TypePointer(spv::StorageClass::Output, t_v4float);

    const auto out_color = m.AddGlobalVariable(t_ptr_out_v4, spv::StorageClass::Output);
    m.Decorate(out_color, spv::Decoration::Location, std::uint32_t{0});

    const auto c_0 = m.Constant(t_float, 0.0f);
    const auto c_1 = m.Constant(t_float, 1.0f);
    const auto color = m.ConstantComposite(t_v4float, c_1, c_0, c_0, c_1);

    const auto main_fn = m.OpFunction(t_void, spv::FunctionControlMask::MaskNone, t_fn);
    m.AddLabel();
    m.OpStore(out_color, color);
    m.OpReturn();
    m.OpFunctionEnd();

    m.AddEntryPoint(spv::ExecutionModel::Fragment, main_fn, "main", out_color);
    m.AddExecutionMode(main_fn, spv::ExecutionMode::OriginUpperLeft);

    return m.Assemble();
}

void test_fragment_shader_replicates_reference() {
    const auto reference = AssembleText(kFragmentSpvText, SPV_ENV_VULKAN_1_0);
    if (!reference.ok) {
        std::fputs(reference.error.c_str(), stderr);
    }
    CHECK(reference.ok);
    if (!reference.ok) return;

    const auto ours = BuildFragmentShaderWithSirit();

    const auto ref_val = Validate(reference.words, SPV_ENV_VULKAN_1_0);
    if (!ref_val.ok) std::fputs(ref_val.messages.c_str(), stderr);
    CHECK(ref_val.ok);

    const auto our_val = Validate(ours, SPV_ENV_VULKAN_1_0);
    if (!our_val.ok) std::fputs(our_val.messages.c_str(), stderr);
    CHECK(our_val.ok);

    const auto ref_sum = Summarize(reference.words);
    const auto our_sum = Summarize(ours);
    const auto diff = SummaryDiff(ref_sum, our_sum);
    if (!diff.empty()) {
        std::fputs(diff.c_str(), stderr);
    }
    CHECK(ref_sum == our_sum);
}

}  // namespace

namespace sirit_tests {

void RegisterFragmentShaderReplicationTests() {
    RUN_TEST(test_fragment_shader_replicates_reference);
}

}  // namespace sirit_tests
