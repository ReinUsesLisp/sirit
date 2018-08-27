/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "sirit/sirit.h"
#include "opcodes.h"

namespace Sirit {

const Op* Module::Function(const Op* result_type, spv::FunctionControlMask function_control,
                            const Op* function_type) {
    Op* op{new Op{spv::Op::OpFunction, bound++, result_type}};
    op->Add(static_cast<u32>(function_control));
    op->Add(function_type);
    return AddCode(op);
}

const Op* Module::FunctionEnd() {
    return AddCode(spv::Op::OpFunctionEnd);
}

} // namespace Sirit
