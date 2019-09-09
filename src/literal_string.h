/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 */

#pragma once

#include <string>
#include "operand.h"
#include "stream.h"

namespace Sirit {

class LiteralString : public Operand {
public:
    LiteralString(std::string string);
    ~LiteralString() override;

    void Fetch(Stream& stream) const override;
    u16 GetWordCount() const override;

    bool operator==(const Operand& other) const override;

private:
    const std::string string;
};

} // namespace Sirit
