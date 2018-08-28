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

} // namespace Sirit
