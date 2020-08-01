/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#include "sirit/sirit.h"

#include "stream.h"

namespace Sirit {

Id Module::OpSubgroupBallotKHR(Id result_type, Id predicate) {
    code->Reserve(4);
    return *code << OpId{spv::Op::OpSubgroupBallotKHR, result_type} << predicate << EndOp{};
}

Id Module::OpSubgroupReadInvocationKHR(Id result_type, Id value, Id index) {
    code->Reserve(5);
    return *code << OpId{spv::Op::OpSubgroupReadInvocationKHR, result_type} << value << index
                 << EndOp{};
}

Id Module::OpSubgroupAllKHR(Id result_type, Id predicate) {
    code->Reserve(4);
    return *code << OpId{spv::Op::OpSubgroupAllKHR, result_type} << predicate << EndOp{};
}

Id Module::OpSubgroupAnyKHR(Id result_type, Id predicate) {
    code->Reserve(4);
    return *code << OpId{spv::Op::OpSubgroupAnyKHR, result_type} << predicate << EndOp{};
}

Id Module::OpSubgroupAllEqualKHR(Id result_type, Id predicate) {
    code->Reserve(4);
    return *code << OpId{spv::Op::OpSubgroupAllEqualKHR, result_type} << predicate << EndOp{};
}

Id Module::OpGroupNonUniformShuffleXor(Id result_type, spv::Scope scope, Id value, Id mask) {
    code->Reserve(6);
    return *code << OpId{spv::Op::OpGroupNonUniformShuffleXor, result_type} << scope << value
                 << mask << EndOp{};
}

} // namespace Sirit
