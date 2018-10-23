/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"

namespace Sirit {

Ref Module::Variable(Ref result_type, spv::StorageClass storage_class,
                     Ref initializer) {
    auto op{new Op(spv::Op::OpVariable, bound++, result_type)};
    AddEnum(op, storage_class);
    if (initializer) {
        op->Add(initializer);
    }
    return AddCode(op);
}

} // namespace Sirit
