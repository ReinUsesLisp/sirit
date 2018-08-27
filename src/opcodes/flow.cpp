/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2 or any later version.
 */

#include "sirit/sirit.h"
#include "opcodes.h"

namespace Sirit {

const Op* Module::Label() {
    return AddCode(spv::Op::OpLabel, bound++);
}

const Op* Module::Return() {
    return AddCode(spv::Op::OpReturn);
}

} // namespace Sirit
