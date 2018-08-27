/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2 or any later version.
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

} // namespace Sirit
