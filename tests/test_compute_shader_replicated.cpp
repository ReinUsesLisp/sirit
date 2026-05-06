/* Compute-shader replication test.
 *
 * Spec: a compute shader (LocalSize 64, 1, 1) that writes the constant 42 to
 * SSBO[gl_GlobalInvocationID.x]. Demonstrates the StorageBuffer storage class
 * (requires SPIR-V 1.3 baseline), a runtime-array-of-uint inside a struct
 * decorated as Block, descriptor-set / binding decorations, and a chained
 * OpAccessChain.
 */

#include <cstdint>
#include <string>
#include <vector>

#include <sirit/sirit.h>

#include "test_helpers.h"

using namespace sirit_tests;

namespace {

// SPIR-V 1.4 -- needed because OpEntryPoint's interface list contains the
// StorageBuffer SSBO variable. Earlier versions restrict the interface to
// Input/Output variables only.
constexpr std::uint32_t SpvVersion14 = 0x00010400;

const char* kComputeSpvText = R"(
               OpCapability Shader
               OpMemoryModel Logical GLSL450
               OpEntryPoint GLCompute %main "main" %gl_id %ssbo
               OpExecutionMode %main LocalSize 64 1 1
               OpDecorate %gl_id BuiltIn GlobalInvocationId
               OpDecorate %arr_uint ArrayStride 4
               OpDecorate %SSBO Block
               OpMemberDecorate %SSBO 0 Offset 0
               OpDecorate %ssbo DescriptorSet 0
               OpDecorate %ssbo Binding 0
       %void = OpTypeVoid
         %fn = OpTypeFunction %void
       %uint = OpTypeInt 32 0
     %v3uint = OpTypeVector %uint 3
%_ptr_Input_v3uint = OpTypePointer Input %v3uint
      %gl_id = OpVariable %_ptr_Input_v3uint Input
%_ptr_Input_uint = OpTypePointer Input %uint
       %u_0  = OpConstant %uint 0
       %u_42 = OpConstant %uint 42
   %arr_uint = OpTypeRuntimeArray %uint
       %SSBO = OpTypeStruct %arr_uint
%_ptr_StorageBuffer_SSBO = OpTypePointer StorageBuffer %SSBO
       %ssbo = OpVariable %_ptr_StorageBuffer_SSBO StorageBuffer
%_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
       %main = OpFunction %void None %fn
      %entry = OpLabel
    %idx_ptr = OpAccessChain %_ptr_Input_uint %gl_id %u_0
        %idx = OpLoad %uint %idx_ptr
    %out_ptr = OpAccessChain %_ptr_StorageBuffer_uint %ssbo %u_0 %idx
                OpStore %out_ptr %u_42
                OpReturn
                OpFunctionEnd
)";

std::vector<std::uint32_t> BuildComputeShaderWithSirit() {
    Sirit::Module m{SpvVersion14};

    m.AddCapability(spv::Capability::Shader);
    m.SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::GLSL450);

    const auto t_void = m.TypeVoid();
    const auto t_fn = m.TypeFunction(t_void);
    const auto t_uint = m.TypeInt(32, false);
    const auto t_v3uint = m.TypeVector(t_uint, 3);
    const auto t_ptr_in_v3uint = m.TypePointer(spv::StorageClass::Input, t_v3uint);
    const auto t_ptr_in_uint = m.TypePointer(spv::StorageClass::Input, t_uint);
    const auto t_arr_uint = m.TypeRuntimeArray(t_uint);
    const auto t_ssbo = m.TypeStruct(t_arr_uint);
    const auto t_ptr_sb_ssbo = m.TypePointer(spv::StorageClass::StorageBuffer, t_ssbo);
    const auto t_ptr_sb_uint = m.TypePointer(spv::StorageClass::StorageBuffer, t_uint);

    const auto gl_id = m.AddGlobalVariable(t_ptr_in_v3uint, spv::StorageClass::Input);
    const auto ssbo = m.AddGlobalVariable(t_ptr_sb_ssbo, spv::StorageClass::StorageBuffer);

    m.Decorate(gl_id, spv::Decoration::BuiltIn,
               static_cast<std::uint32_t>(spv::BuiltIn::GlobalInvocationId));
    m.Decorate(t_arr_uint, spv::Decoration::ArrayStride, std::uint32_t{4});
    m.Decorate(t_ssbo, spv::Decoration::Block);
    m.MemberDecorate(t_ssbo, 0, spv::Decoration::Offset, std::uint32_t{0});
    m.Decorate(ssbo, spv::Decoration::DescriptorSet, std::uint32_t{0});
    m.Decorate(ssbo, spv::Decoration::Binding, std::uint32_t{0});

    const auto u_0 = m.Constant(t_uint, std::uint32_t{0});
    const auto u_42 = m.Constant(t_uint, std::uint32_t{42});

    const auto main_fn = m.OpFunction(t_void, spv::FunctionControlMask::MaskNone, t_fn);
    m.AddLabel();
    const auto idx_ptr = m.OpAccessChain(t_ptr_in_uint, gl_id, u_0);
    const auto idx = m.OpLoad(t_uint, idx_ptr);
    const auto out_ptr = m.OpAccessChain(t_ptr_sb_uint, ssbo, u_0, idx);
    m.OpStore(out_ptr, u_42);
    m.OpReturn();
    m.OpFunctionEnd();

    m.AddEntryPoint(spv::ExecutionModel::GLCompute, main_fn, "main", gl_id, ssbo);
    m.AddExecutionMode(main_fn, spv::ExecutionMode::LocalSize, std::uint32_t{64},
                       std::uint32_t{1}, std::uint32_t{1});

    return m.Assemble();
}

void test_compute_shader_replicates_reference() {
    const auto reference = AssembleText(kComputeSpvText, SPV_ENV_VULKAN_1_2);
    if (!reference.ok) {
        std::fputs(reference.error.c_str(), stderr);
    }
    CHECK(reference.ok);
    if (!reference.ok) return;

    const auto ours = BuildComputeShaderWithSirit();

    const auto ref_val = Validate(reference.words, SPV_ENV_VULKAN_1_2);
    if (!ref_val.ok) std::fputs(ref_val.messages.c_str(), stderr);
    CHECK(ref_val.ok);

    const auto our_val = Validate(ours, SPV_ENV_VULKAN_1_2);
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

void RegisterComputeShaderReplicationTests() {
    RUN_TEST(test_compute_shader_replicates_reference);
}

}  // namespace sirit_tests
