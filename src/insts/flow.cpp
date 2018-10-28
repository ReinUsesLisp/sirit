/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"

namespace Sirit {

Ref Module::LoopMerge(Ref merge_block, Ref continue_target,
                      spv::LoopControlMask loop_control,
                      const std::vector<Ref>& literals) {
    auto op{new Op(spv::Op::OpLoopMerge)};
    op->Add(merge_block);
    op->Add(continue_target);
    AddEnum(op, loop_control);
    op->Add(literals);
    return AddCode(op);
}

Ref Module::SelectionMerge(Ref merge_block,
                           spv::SelectionControlMask selection_control) {
    auto op{new Op(spv::Op::OpSelectionMerge)};
    op->Add(merge_block);
    AddEnum(op, selection_control);
    return AddCode(op);
}

Ref Module::Label() { return AddCode(spv::Op::OpLabel, bound++); }

Ref Module::Branch(Ref target_label) {
    auto op{new Op(spv::Op::OpBranch)};
    op->Add(target_label);
    return AddCode(op);
}

Ref Module::BranchConditional(Ref condition, Ref true_label, Ref false_label,
                              std::uint32_t true_weight,
                              std::uint32_t false_weight) {
    auto op{new Op(spv::Op::OpBranchConditional)};
    op->Add(condition);
    op->Add(true_label);
    op->Add(false_label);
    if (true_weight != 0 || false_weight != 0) {
        op->Add(true_weight);
        op->Add(false_weight);
    }
    return AddCode(op);
}

Ref Module::Return() { return AddCode(spv::Op::OpReturn); }

} // namespace Sirit
