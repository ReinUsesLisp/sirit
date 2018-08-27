/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2 or any later version.
 */

#pragma once

#include "stream.h"
#include "op.h"

namespace Sirit {

template<typename T>
inline void WriteEnum(Stream& stream, spv::Op opcode, T value) {
    Op op{opcode};
    op.Add(static_cast<u32>(value));
    op.Write(stream);
}

} // namespace Sirit
