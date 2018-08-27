/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <cassert>
#include "sirit/sirit.h"
#include "opcodes.h"

namespace Sirit {

const Op* Module::ConstantTrue(const Op* result_type) {
    return AddDeclaration(new Op(spv::Op::OpConstantTrue, bound, result_type));
}

const Op* Module::ConstantFalse(const Op* result_type) {
    return AddDeclaration(new Op(spv::Op::OpConstantFalse, bound, result_type));
}

const Op* Module::Constant(const Op* result_type, Operand* literal) {
    Op* op{new Op(spv::Op::OpConstant, bound, result_type)};
    op->Add(literal);
    return AddDeclaration(op);
}

const Op* Module::ConstantComposite(const Op* result_type,
                                    const std::vector<const Op*>& constituents) {
    Op* op{new Op(spv::Op::OpConstantComposite, bound, result_type)};
    op->Add(constituents);
    return AddDeclaration(op);
}

} // namespace Sirit
