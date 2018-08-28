/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include <string>
#include "stream.h"
#include "operand.h"

namespace Sirit {

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
