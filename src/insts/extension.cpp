/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "common_types.h"
#include "op.h"
#include "sirit/sirit.h"
#include <memory>
#include <spirv/unified1/GLSL.std.450.h>

namespace Sirit {

Id Module::OpExtInst(Id result_type, Id set, u32 instruction,
                     const std::vector<Id>& operands) {
    auto op{std::make_unique<Op>(spv::Op::OpExtInst, bound++, result_type)};
    op->Add(set);
    op->Add(instruction);
    op->Add(operands);
    return AddCode(std::move(op));
}

Id Module::OpFAbs(Id result_type, Id x) {
    return OpExtInst(result_type, GetGLSLstd450(), GLSLstd450FAbs, {x});
}

} // namespace Sirit