/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#include <cassert>
#include "operand.h"

namespace Sirit {

Operand::Operand() = default;

Operand::~Operand() = default;

void Operand::Fetch([[maybe_unused]] Stream& stream) const {
    assert(!"Fetching unimplemented operand");
}

u16 Operand::GetWordCount() const {
    assert(!"Fetching unimplemented operand");
    return 0;
}

bool Operand::operator==([[maybe_unused]] const Operand& other) const {
    return false;
}

bool Operand::operator!=(const Operand& other) const {
    return !(*this == other);
}

OperandType Operand::GetType() const {
    return operand_type;
}

} // namespace Sirit
