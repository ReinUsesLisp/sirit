/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
 */

#pragma once

#include <cstring>
#include <typeindex>
#include "operand.h"
#include "stream.h"

namespace Sirit {

class LiteralNumber : public Operand {
public:
    LiteralNumber(std::type_index type);
    ~LiteralNumber();

    virtual void Fetch(Stream& stream) const;
    virtual u16 GetWordCount() const;

    virtual bool operator==(const Operand& other) const;

    template <typename T>
    static LiteralNumber* Create(T value) {
        static_assert(sizeof(T) == 4 || sizeof(T) == 8);

        LiteralNumber* number = new LiteralNumber(std::type_index(typeid(T)));
        number->is_32 = sizeof(T) == 4;
        std::memcpy(&number->raw, &value, sizeof(T));
        return number;
    }

private:
    std::type_index type;
    bool is_32{};
    u64 raw{};
};

} // namespace Sirit
