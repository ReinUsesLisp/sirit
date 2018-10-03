/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include "operand.h"
#include "stream.h"
#include <typeindex>

namespace Sirit {

class LiteralNumber : public Operand {
  public:
    LiteralNumber(std::type_index type);
    ~LiteralNumber();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;

    template <typename T> static LiteralNumber* Create(T value) {
        static_assert(sizeof(T) == 4 || sizeof(T) == 8);
        LiteralNumber* number = new LiteralNumber(std::type_index(typeid(T)));
        if (number->is_32 = sizeof(T) == 4; number->is_32) {
            number->raw = *reinterpret_cast<u32*>(&value);
        } else {
            number->raw = *reinterpret_cast<u64*>(&value);
        }
        return number;
    }

  private:
    std::type_index type;
    bool is_32;
    u64 raw;
};

} // namespace Sirit
