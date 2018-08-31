/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "sirit/sirit.h"
#include "insts.h"

namespace Sirit {

void Module::Name(Ref target, const std::string& name) {
    Op* op{new Op(spv::Op::OpName)};
    op->Add(target);
    op->Add(name);
    debug.push_back(std::unique_ptr<Op>(op));
}

} // namespace Sirit
