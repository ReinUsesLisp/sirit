/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 *
 * Hand-written control-flow instructions whose emit shape can't be expressed
 * by the simple `*stream << HEAD << op... << EndOp{}` template that
 * tools/generate_instructions.py uses. Everything else in `Flow` lives in
 * _generated.cpp.
 */

#include <cassert>

#include "sirit/sirit.h"

#include "stream.h"

namespace Sirit {

Id Module::DeferredOpPhi(Id result_type, std::span<const Id> blocks) {
    deferred_phi_nodes.push_back(code->LocalAddress());
    code->Reserve(3 + blocks.size() * 2);
    *code << OpId{spv::Op::OpPhi, result_type};
    for (const Id block : blocks) {
        *code << u32{0} << block;
    }
    return *code << EndOp{};
}

Id Module::OpLabel() {
    return Id{++bound};
}

Id Module::OpBranchConditional(Id condition, Id true_label, Id false_label, u32 true_weight,
                               u32 false_weight) {
    code->Reserve(6);
    *code << spv::Op::OpBranchConditional << condition << true_label << false_label;
    if (true_weight != 0 || false_weight != 0) {
        *code << true_weight << false_weight;
    }
    return *code << EndOp{};
}

Id Module::OpSwitch(Id selector, Id default_label, std::span<const Literal> literals,
                    std::span<const Id> labels) {
    assert(literals.size() == labels.size());
    const size_t size = literals.size();
    code->Reserve(3 + size * 2);

    *code << spv::Op::OpSwitch << selector << default_label;
    for (std::size_t i = 0; i < size; ++i) {
        *code << literals[i] << labels[i];
    }
    return *code << EndOp{};
}

void Module::OpDemoteToHelperInvocationEXT() {
    OpDemoteToHelperInvocation();
}

} // namespace Sirit
