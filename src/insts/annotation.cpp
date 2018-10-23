/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "insts.h"
#include "sirit/sirit.h"

namespace Sirit {

Ref Module::Decorate(Ref target, spv::Decoration decoration,
                     const std::vector<Operand*>& literals) {
    auto op{new Op(spv::Op::OpDecorate)};
    op->Add(target);
    AddEnum(op, decoration);
    op->Sink(literals);
    return AddAnnotation(op);
}

} // namespace Sirit
