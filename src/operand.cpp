/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
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

LiteralInteger::LiteralInteger(u32 integer_)
    : integer(integer_) {
    operand_type = OperandType::Integer;
}

LiteralInteger::~LiteralInteger() = default;

void LiteralInteger::Fetch(Stream& stream) const {
    stream.Write(integer);
}

u16 LiteralInteger::GetWordCount() const {
    return 1;
}

bool LiteralInteger::operator==(const Operand& other) const {
    if (operand_type == other.GetType()) {
        return dynamic_cast<const LiteralInteger&>(other).integer == integer;
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
