/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <typeindex>
#include "operand.h"
#include "stream.h"

namespace Sirit {

class LiteralNumber : public Operand {
public:
    LiteralNumber(std::type_index type);
    ~LiteralNumber() override;

    void Fetch(Stream& stream) const override;
    u16 GetWordCount() const override;

    bool operator==(const Operand& other) const override;

    std::size_t Hash() const override;

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
