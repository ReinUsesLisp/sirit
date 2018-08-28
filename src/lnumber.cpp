/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <cassert>
#include "lnumber.h"

namespace Sirit {

LiteralNumber::LiteralNumber() {
    operand_type = OperandType::Number;
}

LiteralNumber::LiteralNumber(u32 number)
    : uint32(number), type(NumberType::U32) {
    LiteralNumber();
}

LiteralNumber::LiteralNumber(s32 number)
    : int32(number), type(NumberType::S32) {
    LiteralNumber();
}

LiteralNumber::LiteralNumber(f32 number)
    : float32(number), type(NumberType::F32) {
    LiteralNumber();
}

LiteralNumber::LiteralNumber(u64 number)
    : uint64(number), type(NumberType::U64) {
    LiteralNumber();
}

LiteralNumber::LiteralNumber(s64 number)
    : int64(number), type(NumberType::S64) {
    LiteralNumber();
}

LiteralNumber::LiteralNumber(f64 number)
    : float64(number), type(NumberType::F64) {
    LiteralNumber();
}

LiteralNumber::~LiteralNumber() = default;

void LiteralNumber::Fetch(Stream& stream) const {
    switch (type) {
        case NumberType::S32:
        case NumberType::U32:
        case NumberType::F32:
            stream.Write(uint32);
            break;
        case NumberType::S64:
        case NumberType::U64:
        case NumberType::F64:
            stream.Write(uint64);
            break;
        default:
            assert(0);
    }
}

u16 LiteralNumber::GetWordCount() const {
    switch (type) {
        case NumberType::S32:
        case NumberType::U32:
        case NumberType::F32:
            return 1;
        case NumberType::S64:
        case NumberType::U64:
        case NumberType::F64:
            return 2;
        default:
            assert(0);
            return 0;
    }
}

bool LiteralNumber::operator==(const Operand& other) const {
    if (operand_type == other.GetType()) {
        const auto& o{dynamic_cast<const LiteralNumber&>(other)};
        return o.type == type && o.raw == raw;
    }
    return false;
}

} // namespace Sirit
