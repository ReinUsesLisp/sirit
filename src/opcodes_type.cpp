/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include "sirit/sirit.h"
#include "opcodes.h"

namespace Sirit {

const Op* Module::TypeVoid() {
    return AddDeclaration(new Op(spv::Op::OpTypeVoid, bound));
}

const Op* Module::TypeFunction(const Op* return_type, const std::vector<const Op*>& arguments) {
    Op* type_func{new Op(spv::Op::OpTypeFunction, bound)};
    type_func->Add(return_type);
    for (const Op* arg : arguments) {
        type_func->Add(arg);
    }
    return AddDeclaration(type_func);
}

} // namespace Sirit
