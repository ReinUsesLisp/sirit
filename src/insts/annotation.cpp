/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"

namespace Sirit {

Id Module::Decorate(Id target, spv::Decoration decoration,
                     const std::vector<Literal>& literals) {
    auto op{new Op(spv::Op::OpDecorate)};
    op->Add(target);
    AddEnum(op, decoration);
    op->Add(literals);
    return AddAnnotation(op);
}

Id Module::MemberDecorate(Id structure_type, Literal member,
                           spv::Decoration decoration,
                           const std::vector<Literal>& literals) {
    auto op{new Op(spv::Op::OpMemberDecorate)};
    op->Add(structure_type);
    op->Add(member);
    AddEnum(op, decoration);
    op->Add(literals);
    return AddAnnotation(op);
}

} // namespace Sirit
