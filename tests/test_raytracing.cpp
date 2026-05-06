/* Ray-tracing shader generation tests.
 *
 * Build a ray-generation shader that:
 *   1. declares an OpTypeAccelerationStructureKHR + bound UniformConstant
 *      acceleration-structure variable;
 *   2. declares a RayPayloadKHR vec4 variable;
 *   3. issues an OpTraceRayKHR call against the loaded accel.
 *
 * Goal: the produced module passes spirv-val under SPV_ENV_VULKAN_1_3 and the
 * disassembly contains the expected RT opcodes / capabilities / extensions.
 */

#include <cstdint>
#include <string>
#include <vector>

#include <sirit/sirit.h>

#include "test_helpers.h"

using namespace sirit_tests;

namespace {

constexpr std::uint32_t SpvVersion14 = 0x00010400;  // SPIR-V 1.4 (Vulkan 1.2 baseline for KHR RT)

std::vector<std::uint32_t> BuildRayGenModule() {
    Sirit::Module m{SpvVersion14};

    m.AddCapability(spv::Capability::RayTracingKHR);
    m.AddExtension("SPV_KHR_ray_tracing");
    m.SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::GLSL450);

    const auto t_void = m.TypeVoid();
    const auto t_uint = m.TypeInt(32, false);
    const auto t_float = m.TypeFloat(32);
    const auto t_v3float = m.TypeVector(t_float, 3);
    const auto t_v4float = m.TypeVector(t_float, 4);

    const auto t_accel = m.TypeAccelerationStructureKHR();
    const auto t_ptr_uc_accel = m.TypePointer(spv::StorageClass::UniformConstant, t_accel);
    const auto t_ptr_payload = m.TypePointer(spv::StorageClass::RayPayloadKHR, t_v4float);

    const auto accel_var = m.AddGlobalVariable(t_ptr_uc_accel, spv::StorageClass::UniformConstant);
    m.Name(accel_var, "topLevelAS");
    m.Decorate(accel_var, spv::Decoration::DescriptorSet, std::uint32_t{0});
    m.Decorate(accel_var, spv::Decoration::Binding, std::uint32_t{0});

    const auto payload_var = m.AddGlobalVariable(t_ptr_payload, spv::StorageClass::RayPayloadKHR);
    m.Name(payload_var, "payload");
    m.Decorate(payload_var, spv::Decoration::Location, std::uint32_t{0});

    const auto cf0 = m.Constant(t_float, 0.0f);
    const auto cf1 = m.Constant(t_float, 1.0f);
    const auto cf1000 = m.Constant(t_float, 1000.0f);
    const auto cu0 = m.Constant(t_uint, std::uint32_t{0});
    const auto cu_ff = m.Constant(t_uint, std::uint32_t{0xff});

    const auto origin = m.ConstantComposite(t_v3float, cf0, cf0, cf0);
    const auto direction = m.ConstantComposite(t_v3float, cf0, cf0, cf1);

    const auto t_func = m.TypeFunction(t_void);
    const auto fn = m.OpFunction(t_void, spv::FunctionControlMask::MaskNone, t_func);
    m.Name(fn, "main");
    m.AddLabel();

    const auto loaded_accel = m.OpLoad(t_accel, accel_var);
    m.OpTraceRayKHR(loaded_accel,
                    /*ray_flags=*/cu0,
                    /*cull_mask=*/cu_ff,
                    /*sbt_offset=*/cu0,
                    /*sbt_stride=*/cu0,
                    /*miss_index=*/cu0,
                    /*ray_origin=*/origin,
                    /*ray_tmin=*/cf0,
                    /*ray_direction=*/direction,
                    /*ray_tmax=*/cf1000,
                    /*payload=*/payload_var);

    m.OpReturn();
    m.OpFunctionEnd();

    m.AddEntryPoint(spv::ExecutionModel::RayGenerationKHR, fn, "main", accel_var, payload_var);

    return m.Assemble();
}

void test_raygen_module_validates() {
    const auto words = BuildRayGenModule();
    const auto result = Validate(words, SPV_ENV_VULKAN_1_3);
    if (!result.ok) {
        std::fputs(result.messages.c_str(), stderr);
    }
    CHECK(result.ok);
}

void test_raygen_disassembly_contains_expected_ops() {
    const auto words = BuildRayGenModule();
    const std::string text = Disassemble(words, SPV_ENV_VULKAN_1_3);
    CHECK(text.find("OpTypeAccelerationStructureKHR") != std::string::npos);
    CHECK(text.find("OpTraceRayKHR") != std::string::npos);
    CHECK(text.find("RayTracingKHR") != std::string::npos);             // capability
    CHECK(text.find("\"SPV_KHR_ray_tracing\"") != std::string::npos);   // extension
    CHECK(text.find("RayGenerationKHR") != std::string::npos);          // execution model
    CHECK(text.find("RayPayloadKHR") != std::string::npos);             // storage class
}

}  // namespace

namespace sirit_tests {

void RegisterRayTracingTests() {
    RUN_TEST(test_raygen_module_validates);
    RUN_TEST(test_raygen_disassembly_contains_expected_ops);
}

}  // namespace sirit_tests
