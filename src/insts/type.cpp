/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <memory>
#include <cassert>
#include <optional>

#include "op.h"
#include "sirit/sirit.h"

namespace Sirit {

Id Module::OpTypeVoid() {
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeVoid, bound));
}

Id Module::OpTypeBool() {
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeBool, bound));
}

Id Module::OpTypeInt(int width, bool is_signed) {
    if (width == 8) {
        AddCapability(spv::Capability::Int8);
    } else if (width == 16) {
        AddCapability(spv::Capability::Int16);
    } else if (width == 64) {
        AddCapability(spv::Capability::Int64);
    }
    auto op{std::make_unique<Op>(spv::Op::OpTypeInt, bound)};
    op->Add(width);
    op->Add(is_signed ? 1 : 0);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeFloat(int width) {
    if (width == 16) {
        AddCapability(spv::Capability::Float16);
    } else if (width == 64) {
        AddCapability(spv::Capability::Float64);
    }
    auto op{std::make_unique<Op>(spv::Op::OpTypeFloat, bound)};
    op->Add(width);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeVector(Id component_type, int component_count) {
    assert(component_count >= 2);
    auto op{std::make_unique<Op>(spv::Op::OpTypeVector, bound)};
    op->Add(component_type);
    op->Add(component_count);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeMatrix(Id column_type, int column_count) {
    assert(column_count >= 2);
    AddCapability(spv::Capability::Matrix);
    auto op{std::make_unique<Op>(spv::Op::OpTypeMatrix, bound)};
    op->Add(column_type);
    op->Add(column_count);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeImage(Id sampled_type, spv::Dim dim, int depth, bool arrayed,
                        bool ms, int sampled, spv::ImageFormat image_format,
                        std::optional<spv::AccessQualifier> access_qualifier) {
    switch (dim) {
    case spv::Dim::Dim1D:
        AddCapability(spv::Capability::Sampled1D);
        break;
    case spv::Dim::Cube:
        AddCapability(spv::Capability::Shader);
        break;
    case spv::Dim::Rect:
        AddCapability(spv::Capability::SampledRect);
        break;
    case spv::Dim::Buffer:
        AddCapability(spv::Capability::SampledBuffer);
        break;
    case spv::Dim::SubpassData:
        AddCapability(spv::Capability::InputAttachment);
        break;
    }
    switch (image_format) {
    case spv::ImageFormat::Rgba32f:
    case spv::ImageFormat::Rgba16f:
    case spv::ImageFormat::R32f:
    case spv::ImageFormat::Rgba8:
    case spv::ImageFormat::Rgba8Snorm:
    case spv::ImageFormat::Rgba32i:
    case spv::ImageFormat::Rgba16i:
    case spv::ImageFormat::Rgba8i:
    case spv::ImageFormat::R32i:
    case spv::ImageFormat::Rgba32ui:
    case spv::ImageFormat::Rgba16ui:
    case spv::ImageFormat::Rgba8ui:
    case spv::ImageFormat::R32ui:
        AddCapability(spv::Capability::Shader);
        break;
    case spv::ImageFormat::Rg32f:
    case spv::ImageFormat::Rg16f:
    case spv::ImageFormat::R11fG11fB10f:
    case spv::ImageFormat::R16f:
    case spv::ImageFormat::Rgba16:
    case spv::ImageFormat::Rgb10A2:
    case spv::ImageFormat::Rg16:
    case spv::ImageFormat::Rg8:
    case spv::ImageFormat::R16:
    case spv::ImageFormat::R8:
    case spv::ImageFormat::Rgba16Snorm:
    case spv::ImageFormat::Rg16Snorm:
    case spv::ImageFormat::Rg8Snorm:
    case spv::ImageFormat::Rg32i:
    case spv::ImageFormat::Rg16i:
    case spv::ImageFormat::Rg8i:
    case spv::ImageFormat::R16i:
    case spv::ImageFormat::R8i:
    case spv::ImageFormat::Rgb10a2ui:
    case spv::ImageFormat::Rg32ui:
    case spv::ImageFormat::Rg16ui:
    case spv::ImageFormat::Rg8ui:
    case spv::ImageFormat::R16ui:
    case spv::ImageFormat::R8ui:
        AddCapability(spv::Capability::StorageImageExtendedFormats);
        break;
    }
    auto op{std::make_unique<Op>(spv::Op::OpTypeImage, bound)};
    op->Add(sampled_type);
    op->Add(static_cast<u32>(dim));
    op->Add(depth);
    op->Add(arrayed ? 1 : 0);
    op->Add(ms ? 1 : 0);
    op->Add(sampled);
    op->Add(static_cast<u32>(image_format));
    if (access_qualifier.has_value()) {
        AddCapability(spv::Capability::Kernel);
        op->Add(static_cast<u32>(access_qualifier.value()));
    }
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeSampler() {
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeSampler, bound));
}

Id Module::OpTypeSampledImage(Id image_type) {
    auto op{std::make_unique<Op>(spv::Op::OpTypeSampledImage, bound)};
    op->Add(image_type);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeArray(Id element_type, Id length) {
    auto op{std::make_unique<Op>(spv::Op::OpTypeArray, bound)};
    op->Add(element_type);
    op->Add(length);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeRuntimeArray(Id element_type) {
    AddCapability(spv::Capability::Shader);
    auto op{std::make_unique<Op>(spv::Op::OpTypeRuntimeArray, bound)};
    op->Add(element_type);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeStruct(const std::vector<Id>& members) {
    auto op{std::make_unique<Op>(spv::Op::OpTypeStruct, bound)};
    op->Add(members);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeOpaque(const std::string& name) {
    AddCapability(spv::Capability::Kernel);
    auto op{std::make_unique<Op>(spv::Op::OpTypeOpaque, bound)};
    op->Add(name);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypePointer(spv::StorageClass storage_class, Id type) {
    switch (storage_class) {
    case spv::StorageClass::Uniform:
    case spv::StorageClass::Output:
    case spv::StorageClass::Private:
    case spv::StorageClass::PushConstant:
    case spv::StorageClass::StorageBuffer:
        AddCapability(spv::Capability::Shader);
        break;
    case spv::StorageClass::Generic:
        AddCapability(spv::Capability::GenericPointer);
        break;
    case spv::StorageClass::AtomicCounter:
        AddCapability(spv::Capability::AtomicStorage);
        break;
    }
    auto op{std::make_unique<Op>(spv::Op::OpTypePointer, bound)};
    op->Add(static_cast<u32>(storage_class));
    op->Add(type);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeFunction(Id return_type, const std::vector<Id>& arguments) {
    auto op{std::make_unique<Op>(spv::Op::OpTypeFunction, bound)};
    op->Add(return_type);
    op->Add(arguments);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeEvent() {
    AddCapability(spv::Capability::Kernel);
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeEvent, bound));
}

Id Module::OpTypeDeviceEvent() {
    AddCapability(spv::Capability::DeviceEnqueue);
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeDeviceEvent, bound));
}

Id Module::OpTypeReserveId() {
    AddCapability(spv::Capability::Pipes);
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeReserveId, bound));
}

Id Module::OpTypeQueue() {
    AddCapability(spv::Capability::DeviceEnqueue);
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeQueue, bound));
}

Id Module::OpTypePipe(spv::AccessQualifier access_qualifier) {
    AddCapability(spv::Capability::Pipes);
    auto op{std::make_unique<Op>(spv::Op::OpTypePipe, bound)};
    op->Add(static_cast<u32>(access_qualifier));
    return AddDeclaration(std::move(op));
}

} // namespace Sirit
