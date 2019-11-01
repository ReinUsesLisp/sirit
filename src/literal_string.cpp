/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#include <string>
#include "common_types.h"
#include "literal_string.h"

namespace Sirit {

LiteralString::LiteralString(std::string string) : string{std::move(string)} {
    operand_type = OperandType::String;
}

LiteralString::~LiteralString() = default;

void LiteralString::Fetch(Stream& stream) const {
    stream.Write(string);
}

u16 LiteralString::GetWordCount() const {
    return static_cast<u16>(string.size() / 4 + 1);
}

bool LiteralString::operator==(const Operand& other) const {
    if (operand_type == other.GetType()) {
        return static_cast<const LiteralString&>(other).string == string;
    }
    return false;
}

} // namespace Sirit
