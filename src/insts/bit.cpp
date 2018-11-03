/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "common_types.h"
#include "op.h"
#include "sirit/sirit.h"
#include <memory>

namespace Sirit {

Id Module::OpShiftRightLogical(Id result_type, Id base, Id shift) {
    auto op{std::make_unique<Op>(spv::Op::OpShiftRightLogical, bound++,
                                 result_type)};
    op->Add(base);
    op->Add(shift);
    return AddCode(std::move(op));
}

Id Module::OpShiftRightArithmetic(Id result_type, Id base, Id shift) {
    auto op{std::make_unique<Op>(spv::Op::OpShiftRightArithmetic, bound++,
                                 result_type)};
    op->Add(base);
    op->Add(shift);
    return AddCode(std::move(op));
}

Id Module::OpShiftLeftLogical(Id result_type, Id base, Id shift) {
    auto op{std::make_unique<Op>(spv::Op::OpShiftLeftLogical, bound++,
                                 result_type)};
    op->Add(base);
    op->Add(shift);
    return AddCode(std::move(op));
}

} // namespace Sirit