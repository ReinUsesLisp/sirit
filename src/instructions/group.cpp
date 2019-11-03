/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#include "op.h"
#include "sirit/sirit.h"

namespace Sirit {

Id Module::OpSubgroupReadInvocationKHR(Id result_type, Id value, Id index) {
    auto op = std::make_unique<Op>(spv::Op::OpSubgroupReadInvocationKHR, bound++, result_type);
    op->Add(value);
    op->Add(index);
    return AddCode(std::move(op));
}

Id Module::OpGroupNonUniformShuffleXor(Id result_type, spv::Scope scope, Id value, Id mask) {
    auto op = std::make_unique<Op>(spv::Op::OpGroupNonUniformShuffleXor, bound++, result_type);
    op->Add(static_cast<u32>(scope));
    op->Add(value);
    op->Add(mask);
    return AddCode(std::move(op));
}

} // namespace Sirit
