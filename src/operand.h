/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include "stream.h"

namespace Sirit {

enum class OperandType {
    Invalid,
    Op,
    Number,
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

class LiteralNumber : public Operand {
public:
    LiteralNumber(u32 number);
    LiteralNumber(s32 number);
    LiteralNumber(f32 number);
    LiteralNumber(u64 number);
    LiteralNumber(s64 number);
    LiteralNumber(f64 number);
    ~LiteralNumber();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;

private:
    LiteralNumber();

    enum class NumberType {
        U32,
        S32,
        F32,
        U64,
        S64,
        F64
    } type;

    union {
        u64 raw{};
        u32 uint32;
        s32 int32;
        u64 uint64;
        s64 int64;
        f32 float32;
        f64 float64;
    };
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
