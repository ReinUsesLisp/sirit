/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#pragma once

#include "stream.h"

namespace Sirit {

enum class OperandType { Invalid, Op, Number, String };

class Operand {
public:
    Operand();
    virtual ~Operand();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;
    bool operator!=(const Operand& other) const;

    OperandType GetType() const;

protected:
    OperandType operand_type{};
};

} // namespace Sirit
