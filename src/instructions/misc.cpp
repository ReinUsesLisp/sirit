/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#include "sirit/sirit.h"

#include "stream.h"

namespace Sirit {

Id Module::OpUndef(Id result_type) {
    code->Reserve(3);
    return *code << OpId{spv::Op::OpUndef, result_type} << EndOp{};
}

Id Module::OpEmitVertex() {
    code->Reserve(1);
    return *code << OpId{spv::Op::OpEmitVertex} << EndOp{};
}

Id Module::OpEndPrimitive() {
    code->Reserve(1);
    return *code << OpId{spv::Op::OpEndPrimitive} << EndOp{};
}

} // namespace Sirit
