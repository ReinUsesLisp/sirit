/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "sirit/sirit.h"
#include "insts.h"

namespace Sirit {

Ref Module::LoopMerge(Ref merge_block, Ref continue_target, spv::LoopControlMask loop_control,
                      const std::vector<Ref>& literals) {
    Op* op{new Op(spv::Op::OpLoopMerge)};
    op->Add(merge_block);
    op->Add(continue_target);
    AddEnum(op, loop_control);
    op->Add(literals);
    return AddCode(op);
}

Ref Module::Label() {
    return AddCode(spv::Op::OpLabel, bound++);
}

Ref Module::Return() {
    return AddCode(spv::Op::OpReturn);
}

} // namespace Sirit
