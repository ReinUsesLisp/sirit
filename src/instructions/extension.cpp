/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 *
 * Hand-written extended-instruction helpers whose shape can't be expressed
 * by the wrapper template in tools/generate_instructions.py. Specifically,
 * OpDebugPrintf prepends `format` onto a runtime-built operands vector and
 * routes through TypeVoid(). All other GLSL.std.450 / SPV_AMD_* / NonSemantic
 * wrappers live in _generated.cpp.
 */

#include <iterator>
#include <vector>

#include <spirv/unified1/NonSemanticDebugPrintf.h>

#include "sirit/sirit.h"

#include "stream.h"

namespace Sirit {

Id Module::OpDebugPrintf(Id format, std::span<const Id> fmt_args) {
    std::vector<Id> operands;
    operands.reserve(1 + fmt_args.size());
    operands.push_back(format);
    std::copy(fmt_args.begin(), fmt_args.end(), std::back_inserter(operands));
    return OpExtInst(TypeVoid(), GetNonSemanticDebugPrintf(), NonSemanticDebugPrintfDebugPrintf,
                     operands);
}

} // namespace Sirit
