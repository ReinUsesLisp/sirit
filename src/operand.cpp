/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <cassert>
#include "operand.h"

namespace Sirit {

Operand::Operand() {}

Operand::~Operand() = default;

void Operand::Fetch(Stream& stream) const {
    assert(!"Fetching unimplemented operand");
}

u16 Operand::GetWordCount() const {
    assert(!"Fetching unimplemented operand");
    return 0;
}

bool Operand::operator==(const Operand& other) const {
    return false;
}

bool Operand::operator!=(const Operand& other) const {
    return !(*this == other);
}

OperandType Operand::GetType() const {
    return operand_type;
}

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

LiteralString::LiteralString(const std::string& string_)
    : string(string_) {
    operand_type = OperandType::String;
}

LiteralString::~LiteralString() = default;

void LiteralString::Fetch(Stream& stream) const {
    for (std::size_t i{}; i < string.size(); i++) {
        stream.Write(static_cast<u8>(string[i]));
    }
    for (std::size_t i{}; i < 4 - (string.size() % 4); i++) {
        stream.Write(static_cast<u8>(0));
    }
}

u16 LiteralString::GetWordCount() const {
    return static_cast<u16>(string.size() / 4 + 1);
}

bool LiteralString::operator==(const Operand& other) const {
    if (operand_type == other.GetType()) {
        return dynamic_cast<const LiteralString&>(other).string == string;
    }
    return false;
}

} // namespace Sirit
