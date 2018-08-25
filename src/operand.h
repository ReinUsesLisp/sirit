/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#pragma once

#include "stream.h"

namespace Sirit {

enum class OperandType {
    Invalid,
    Ref,
    Integer,
    String
};

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

class LiteralInteger : public Operand {
public:
    LiteralInteger(u32 integer);
    ~LiteralInteger();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;

private:
    u32 integer;
};

class LiteralString : public Operand {
public:
    LiteralString(const std::string& string);
    ~LiteralString();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;

private:
    std::string string;
};

} // namespace Sirit
