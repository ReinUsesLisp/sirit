/* Vertex-shader replication test.
 *
 * Spec: a pass-through vertex shader that loads a vec4 position from input
 * Location 0 and stores it to gl_Position (gl_PerVertex.Position).
 *
 * Strategy: assemble the reference SPIR-V text via SPIRV-Tools, build the
 * same shader with sirit, validate both, then compare ModuleSummary
 * (capabilities / extensions / entry points / opcode multiset).
 */

#include <cstdint>
#include <string>
#include <vector>

#include <sirit/sirit.h>

#include "test_helpers.h"

using namespace sirit_tests;

namespace {

constexpr std::uint32_t SpvVersion10 = 0x00010000;

const char* kVertexSpvText = R"(
               OpCapability Shader
               OpMemoryModel Logical GLSL450
               OpEntryPoint Vertex %main "main" %_ %in_pos
               OpDecorate %per_vertex Block
               OpMemberDecorate %per_vertex 0 BuiltIn Position
               OpDecorate %in_pos Location 0
       %void = OpTypeVoid
         %fn = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v4float = OpTypeVector %float 4
 %per_vertex = OpTypeStruct %v4float
%_ptr_Output_per_vertex = OpTypePointer Output %per_vertex
          %_ = OpVariable %_ptr_Output_per_vertex Output
%_ptr_Input_v4float = OpTypePointer Input %v4float
     %in_pos = OpVariable %_ptr_Input_v4float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
       %uint = OpTypeInt 32 0
   %c0_uint = OpConstant %uint 0
       %main = OpFunction %void None %fn
      %entry = OpLabel
        %pos = OpLoad %v4float %in_pos
     %gl_pos = OpAccessChain %_ptr_Output_v4float %_ %c0_uint
                OpStore %gl_pos %pos
                OpReturn
                OpFunctionEnd
)";

std::vector<std::uint32_t> BuildVertexShaderWithSirit() {
    Sirit::Module m{SpvVersion10};

    m.AddCapability(spv::Capability::Shader);
    m.SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::GLSL450);

    const auto t_void = m.TypeVoid();
    const auto t_fn = m.TypeFunction(t_void);
    const auto t_float = m.TypeFloat(32);
    const auto t_v4float = m.TypeVector(t_float, 4);
    const auto t_per_vertex = m.TypeStruct(t_v4float);
    const auto t_ptr_out_pv = m.TypePointer(spv::StorageClass::Output, t_per_vertex);
    const auto t_ptr_in_v4 = m.TypePointer(spv::StorageClass::Input, t_v4float);
    const auto t_ptr_out_v4 = m.TypePointer(spv::StorageClass::Output, t_v4float);
    const auto t_uint = m.TypeInt(32, false);

    const auto out_pv = m.AddGlobalVariable(t_ptr_out_pv, spv::StorageClass::Output);
    const auto in_pos = m.AddGlobalVariable(t_ptr_in_v4, spv::StorageClass::Input);

    m.Decorate(t_per_vertex, spv::Decoration::Block);
    m.MemberDecorate(t_per_vertex, 0, spv::Decoration::BuiltIn,
                     static_cast<std::uint32_t>(spv::BuiltIn::Position));
    m.Decorate(in_pos, spv::Decoration::Location, std::uint32_t{0});

    const auto c0 = m.Constant(t_uint, std::uint32_t{0});

    const auto main_fn = m.OpFunction(t_void, spv::FunctionControlMask::MaskNone, t_fn);
    m.AddLabel();
    const auto pos = m.OpLoad(t_v4float, in_pos);
    const auto gl_pos = m.OpAccessChain(t_ptr_out_v4, out_pv, c0);
    m.OpStore(gl_pos, pos);
    m.OpReturn();
    m.OpFunctionEnd();

    m.AddEntryPoint(spv::ExecutionModel::Vertex, main_fn, "main", out_pv, in_pos);

    return m.Assemble();
}

void test_vertex_shader_replicates_reference() {
    const auto reference = AssembleText(kVertexSpvText, SPV_ENV_VULKAN_1_0);
    if (!reference.ok) {
        std::fputs(reference.error.c_str(), stderr);
    }
    CHECK(reference.ok);
    if (!reference.ok) return;

    const auto ours = BuildVertexShaderWithSirit();

    // Both validate.
    const auto ref_val = Validate(reference.words, SPV_ENV_VULKAN_1_0);
    if (!ref_val.ok) std::fputs(ref_val.messages.c_str(), stderr);
    CHECK(ref_val.ok);

    const auto our_val = Validate(ours, SPV_ENV_VULKAN_1_0);
    if (!our_val.ok) std::fputs(our_val.messages.c_str(), stderr);
    CHECK(our_val.ok);

    // Structural equivalence.
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

void RegisterVertexShaderReplicationTests() {
    RUN_TEST(test_vertex_shader_replicates_reference);
}

}  // namespace sirit_tests
