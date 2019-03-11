/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
 */

#include <cassert>
#include <memory>
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
    auto op{std::make_unique<Op>(spv::Op::OpTypeInt, bound)};
    op->Add(width);
    op->Add(is_signed ? 1 : 0);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeFloat(int width) {
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
    auto op{std::make_unique<Op>(spv::Op::OpTypeMatrix, bound)};
    op->Add(column_type);
    op->Add(column_count);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypeImage(Id sampled_type, spv::Dim dim, int depth, bool arrayed, bool ms, int sampled,
                       spv::ImageFormat image_format,
                       std::optional<spv::AccessQualifier> access_qualifier) {
    auto op{std::make_unique<Op>(spv::Op::OpTypeImage, bound)};
    op->Add(sampled_type);
    op->Add(static_cast<u32>(dim));
    op->Add(depth);
    op->Add(arrayed ? 1 : 0);
    op->Add(ms ? 1 : 0);
    op->Add(sampled);
    op->Add(static_cast<u32>(image_format));
    if (access_qualifier.has_value()) {
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
    auto op{std::make_unique<Op>(spv::Op::OpTypeOpaque, bound)};
    op->Add(name);
    return AddDeclaration(std::move(op));
}

Id Module::OpTypePointer(spv::StorageClass storage_class, Id type) {
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
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeEvent, bound));
}

Id Module::OpTypeDeviceEvent() {
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeDeviceEvent, bound));
}

Id Module::OpTypeReserveId() {
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeReserveId, bound));
}

Id Module::OpTypeQueue() {
    return AddDeclaration(std::make_unique<Op>(spv::Op::OpTypeQueue, bound));
}

Id Module::OpTypePipe(spv::AccessQualifier access_qualifier) {
    auto op{std::make_unique<Op>(spv::Op::OpTypePipe, bound)};
    op->Add(static_cast<u32>(access_qualifier));
    return AddDeclaration(std::move(op));
}

} // namespace Sirit
