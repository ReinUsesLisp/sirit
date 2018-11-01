/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"
#include <cassert>

namespace Sirit {

Id Module::OpConstantTrue(Id result_type) {
    return AddDeclaration(new Op(spv::Op::OpConstantTrue, bound, result_type));
}

Id Module::OpConstantFalse(Id result_type) {
    return AddDeclaration(new Op(spv::Op::OpConstantFalse, bound, result_type));
}

Id Module::OpConstant(Id result_type, const Literal& literal) {
    auto op{new Op(spv::Op::OpConstant, bound, result_type)};
    op->Add(literal);
    return AddDeclaration(op);
}

Id Module::OpConstantComposite(Id result_type,
                                const std::vector<Id>& constituents) {
    auto op{new Op(spv::Op::OpConstantComposite, bound, result_type)};
    op->Add(constituents);
    return AddDeclaration(op);
}

Id Module::OpConstantSampler(Id result_type,
                              spv::SamplerAddressingMode addressing_mode,
                              bool normalized,
                              spv::SamplerFilterMode filter_mode) {
    AddCapability(spv::Capability::LiteralSampler);
    AddCapability(spv::Capability::Kernel);
    auto op{new Op(spv::Op::OpConstantSampler, bound, result_type)};
    AddEnum(op, addressing_mode);
    op->Add(normalized ? 1 : 0);
    AddEnum(op, filter_mode);
    return AddDeclaration(op);
}

Id Module::OpConstantNull(Id result_type) {
    return AddDeclaration(new Op(spv::Op::OpConstantNull, bound, result_type));
}

} // namespace Sirit
