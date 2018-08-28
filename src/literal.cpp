/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "sirit/sirit.h"
#include "common_types.h"
#include "operand.h"
#include "lnumber.h"

namespace Sirit {

Operand* Module::Literal(u32 value) {
    return new LiteralNumber(value);
}

Operand* Module::Literal(u64 value) {
    return new LiteralNumber(value);
}

Operand* Module::Literal(s32 value) {
    return new LiteralNumber(value);
}

Operand* Module::Literal(s64 value) {
    return new LiteralNumber(value);
}

Operand* Module::Literal(f32 value) {
    return new LiteralNumber(value);
}

Operand* Module::Literal(f64 value) {
    return new LiteralNumber(value);
}

} // namespace Sirit
