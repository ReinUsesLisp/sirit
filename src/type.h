/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#pragma once

#include <vector>
#include "sirit/sirit.h"
#include "ref.h"

namespace Sirit {

class TypeConstant : public Ref {
public:
    Type(spv::Op opcode, u32 id, std::);
    ~Type();

    bool operator==(const Type& other) const;

private:
    /// Arguments can be type references or constants
    std::vector<Operand> args;

    friend Type;
};

using Type = TypeConstant;

using Constant = TypeConstant;

} // namespace Sirit
