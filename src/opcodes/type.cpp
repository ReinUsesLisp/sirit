/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <cassert>
#include "sirit/sirit.h"
#include "opcodes.h"

namespace Sirit {

Ref Module::TypeVoid() {
    return AddDeclaration(new Op(spv::Op::OpTypeVoid, bound));
}

Ref Module::TypeBool() {
    return AddDeclaration(new Op(spv::Op::OpTypeBool, bound));
}

Ref Module::TypeInt(int width, bool is_signed) {
    if (width == 8) {
        AddCapability(spv::Capability::Int8);
    } else if (width == 16) {
        AddCapability(spv::Capability::Int16);
    } else if (width == 64) {
        AddCapability(spv::Capability::Int64);
    }
    Op* op{new Op(spv::Op::OpTypeInt, bound)};
    op->Add(width);
    op->Add(is_signed ? 1 : 0);
    return AddDeclaration(op);
}

Ref Module::TypeFloat(int width) {
    if (width == 16) {
        AddCapability(spv::Capability::Float16);
    } else if (width == 64) {
        AddCapability(spv::Capability::Float64);
    }
    Op* op{new Op(spv::Op::OpTypeFloat, bound)};
    op->Add(width);
    return AddDeclaration(op);
}

Ref Module::TypeVector(Ref component_type, int component_count) {
    assert(component_count >= 2);
    Op* op{new Op(spv::Op::OpTypeVector, bound)};
    op->Add(component_type);
    op->Add(component_count);
    return AddDeclaration(op);
}

Ref Module::TypeMatrix(Ref column_type, int column_count) {
    assert(column_count >= 2);
    AddCapability(spv::Capability::Matrix);
    Op* op{new Op(spv::Op::OpTypeMatrix, bound)};
    op->Add(column_type);
    op->Add(column_count);
    return AddDeclaration(op);
}

Ref Module::TypeImage(Ref sampled_type, spv::Dim dim, int depth, bool arrayed, bool ms,
                            int sampled, spv::ImageFormat image_format,
                            spv::AccessQualifier access_qualifier) {
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
    Op* op{new Op(spv::Op::OpTypeImage, bound)};
    op->Add(sampled_type);
    op->Add(static_cast<u32>(dim));
    op->Add(depth);
    op->Add(arrayed ? 1 : 0);
    op->Add(ms ? 1 : 0);
    op->Add(sampled);
    op->Add(static_cast<u32>(image_format));
    if (static_cast<u32>(access_qualifier) != Undefined) {
        AddCapability(spv::Capability::Kernel);
        op->Add(static_cast<u32>(access_qualifier));
    }
    return AddDeclaration(op);
}

Ref Module::TypeSampler() {
    return AddDeclaration(new Op(spv::Op::OpTypeSampler, bound));
}

Ref Module::TypeSampledImage(Ref image_type) {
    Op* op{new Op(spv::Op::OpTypeSampledImage, bound)};
    op->Add(image_type);
    return AddDeclaration(op);
}

Ref Module::TypeArray(Ref element_type, Ref length) {
    Op* op{new Op(spv::Op::OpTypeArray, bound)};
    op->Add(element_type);
    op->Add(length);
    return AddDeclaration(op);
}

Ref Module::TypeRuntimeArray(Ref element_type) {
    AddCapability(spv::Capability::Shader);
    Op* op{new Op(spv::Op::OpTypeRuntimeArray, bound)};
    op->Add(element_type);
    return AddDeclaration(op);
}

Ref Module::TypeStruct(const std::vector<Ref>& members) {
    Op* op{new Op(spv::Op::OpTypeStruct, bound)};
    op->Add(members);
    return AddDeclaration(op);
}

Ref Module::TypeOpaque(const std::string& name) {
    AddCapability(spv::Capability::Kernel);
    Op* op{new Op(spv::Op::OpTypeOpaque, bound)};
    op->Add(name);
    return AddDeclaration(op);
}

Ref Module::TypePointer(spv::StorageClass storage_class, Ref type) {
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
    Op* op{new Op(spv::Op::OpTypePointer, bound)};
    op->Add(static_cast<u32>(storage_class));
    op->Add(type);
    return AddDeclaration(op);
}

Ref Module::TypeFunction(Ref return_type, const std::vector<Ref>& arguments) {
    Op* op{new Op(spv::Op::OpTypeFunction, bound)};
    op->Add(return_type);
    op->Add(arguments);
    return AddDeclaration(op);
}

Ref Module::TypeEvent() {
    AddCapability(spv::Capability::Kernel);
    return AddDeclaration(new Op(spv::Op::OpTypeEvent, bound));
}

Ref Module::TypeDeviceEvent() {
    AddCapability(spv::Capability::DeviceEnqueue);
    return AddDeclaration(new Op(spv::Op::OpTypeDeviceEvent, bound));
}

Ref Module::TypeReserveId() {
    AddCapability(spv::Capability::Pipes);
    return AddDeclaration(new Op(spv::Op::OpTypeReserveId, bound));
}

Ref Module::TypeQueue() {
    AddCapability(spv::Capability::DeviceEnqueue);
    return AddDeclaration(new Op(spv::Op::OpTypeQueue, bound));
}

Ref Module::TypePipe(spv::AccessQualifier access_qualifier) {
    AddCapability(spv::Capability::Pipes);
    Op* op{new Op(spv::Op::OpTypePipe, bound)};
    op->Add(static_cast<u32>(access_qualifier));
    return AddDeclaration(op);
}

} // namespace Sirit
