/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
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
