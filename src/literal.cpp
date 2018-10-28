/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "common_types.h"
#include "literal-number.h"
#include "operand.h"
#include "sirit/sirit.h"

namespace Sirit {

#define DEFINE_LITERAL(type)                                                   \
    Operand* Module::Literal(type value) {                                     \
        return LiteralNumber::Create<type>(value);                             \
    }

DEFINE_LITERAL(u32)
DEFINE_LITERAL(u64)
DEFINE_LITERAL(s32)
DEFINE_LITERAL(s64)
DEFINE_LITERAL(f32)
DEFINE_LITERAL(f64)

} // namespace Sirit
