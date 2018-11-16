/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
 */

#include "common_types.h"
#include "op.h"
#include "sirit/sirit.h"

namespace Sirit {

Id Module::OpSampledImage(Id result_type, Id image, Id sampler) {
    auto op{
        std::make_unique<Op>(spv::Op::OpSampledImage, bound++, result_type)};
    op->Add(image);
    op->Add(sampler);
    return AddCode(std::move(op));
}

void AddImageOperands(Op* op,
                      std::optional<spv::ImageOperandsMask> image_operands,
                      const std::vector<Id>& operands) {
    if (!image_operands)
        return;
    op->Add(static_cast<u32>(*image_operands));
    op->Add(operands);
}

#define DEFINE_IMAGE_OP(funcname, opcode)                                      \
    Id Module::funcname(Id result_type, Id sampled_image, Id coordinate,       \
                        std::optional<spv::ImageOperandsMask> image_operands,  \
                        const std::vector<Id>& operands) {                     \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(sampled_image);                                                \
        op->Add(coordinate);                                                   \
        AddImageOperands(op.get(), image_operands, operands);                  \
        return AddCode(std::move(op));                                         \
    }

#define DEFINE_IMAGE_EXP_OP(funcname, opcode)                                  \
    Id Module::funcname(Id result_type, Id sampled_image, Id coordinate,       \
                        spv::ImageOperandsMask image_operands, Id lod,         \
                        const std::vector<Id>& operands) {                     \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(sampled_image);                                                \
        op->Add(coordinate);                                                   \
        op->Add(static_cast<u32>(image_operands));                             \
        op->Add(lod);                                                          \
        op->Add(operands);                                                     \
        return AddCode(std::move(op));                                         \
    }

#define DEFINE_IMAGE_EXTRA_OP(funcname, opcode)                                \
    Id Module::funcname(Id result_type, Id sampled_image, Id coordinate,       \
                        Id extra,                                              \
                        std::optional<spv::ImageOperandsMask> image_operands,  \
                        const std::vector<Id>& operands) {                     \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(sampled_image);                                                \
        op->Add(coordinate);                                                   \
        op->Add(extra);                                                        \
        AddImageOperands(op.get(), image_operands, operands);                  \
        return AddCode(std::move(op));                                         \
    }

#define DEFINE_IMAGE_EXTRA_EXP_OP(funcname, opcode)                            \
    Id Module::funcname(Id result_type, Id sampled_image, Id coordinate,       \
                        Id extra, spv::ImageOperandsMask image_operands,       \
                        Id lod, const std::vector<Id>& operands) {             \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(sampled_image);                                                \
        op->Add(coordinate);                                                   \
        op->Add(extra);                                                        \
        op->Add(static_cast<u32>(image_operands));                             \
        op->Add(lod);                                                          \
        op->Add(operands);                                                     \
        return AddCode(std::move(op));                                         \
    }

DEFINE_IMAGE_OP(OpImageSampleImplicitLod, spv::Op::OpImageSampleImplicitLod)
DEFINE_IMAGE_EXP_OP(OpImageSampleExplicitLod, spv::Op::OpImageSampleExplicitLod)
DEFINE_IMAGE_EXTRA_OP(OpImageSampleDrefImplicitLod,
                      spv::Op::OpImageSampleDrefImplicitLod)
DEFINE_IMAGE_EXTRA_EXP_OP(OpImageSampleDrefExplicitLod,
                          spv::Op::OpImageSampleDrefExplicitLod)
DEFINE_IMAGE_OP(OpImageSampleProjImplicitLod,
                spv::Op::OpImageSampleProjImplicitLod)
DEFINE_IMAGE_EXP_OP(OpImageSampleProjExplicitLod,
                    spv::Op::OpImageSampleProjExplicitLod)
DEFINE_IMAGE_EXTRA_OP(OpImageSampleProjDrefImplicitLod,
                      spv::Op::OpImageSampleProjDrefImplicitLod)
DEFINE_IMAGE_EXTRA_EXP_OP(OpImageSampleProjDrefExplicitLod,
                          spv::Op::OpImageSampleProjDrefExplicitLod)
DEFINE_IMAGE_OP(OpImageFetch, spv::Op::OpImageFetch)
DEFINE_IMAGE_EXTRA_OP(OpImageGather, spv::Op::OpImageGather)
DEFINE_IMAGE_EXTRA_OP(OpImageDrefGather, spv::Op::OpImageDrefGather)
DEFINE_IMAGE_OP(OpImageRead, spv::Op::OpImageRead)

Id Module::OpImageWrite(Id image, Id coordinate, Id texel,
                        std::optional<spv::ImageOperandsMask> image_operands,
                        const std::vector<Id>& operands) {
    auto op{std::make_unique<Op>(spv::Op::OpImageWrite)};
    op->Add(image);
    op->Add(coordinate);
    op->Add(texel);
    AddImageOperands(op.get(), image_operands, operands);
    return AddCode(std::move(op));
}

Id Module::OpImage(Id result_type, Id sampled_image) {
    auto op{std::make_unique<Op>(spv::Op::OpImage, bound++, result_type)};
    op->Add(sampled_image);
    return AddCode(std::move(op));
}

#define DEFINE_IMAGE_QUERY_OP(funcname, opcode)                                \
    Id Module::funcname(Id result_type, Id image) {                            \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(image);                                                        \
        return AddCode(std::move(op));                                         \
    }

#define DEFINE_IMAGE_QUERY_BIN_OP(funcname, opcode)                            \
    Id Module::funcname(Id result_type, Id image, Id extra) {                  \
        auto op{std::make_unique<Op>(opcode, bound++, result_type)};           \
        op->Add(image);                                                        \
        op->Add(extra);                                                        \
        return AddCode(std::move(op));                                         \
    }

DEFINE_IMAGE_QUERY_BIN_OP(OpImageQuerySizeLod, spv::Op::OpImageQuerySizeLod)
DEFINE_IMAGE_QUERY_OP(OpImageQuerySize, spv::Op::OpImageQuerySize)
DEFINE_IMAGE_QUERY_BIN_OP(OpImageQueryLod, spv::Op::OpImageQueryLod)
DEFINE_IMAGE_QUERY_OP(OpImageQueryLevels, spv::Op::OpImageQueryLevels)
DEFINE_IMAGE_QUERY_OP(OpImageQuerySamples, spv::Op::OpImageQuerySamples)

} // namespace Sirit
