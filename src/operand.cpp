/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
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

std::size_t Operand::Hash() const {
    return static_cast<std::size_t>(operand_type) << 30;
}

OperandType Operand::GetType() const {
    return operand_type;
}

} // namespace Sirit
