/* This file is part of the sirit project.
 * Copyright (c) 2019 sirit
 * This software may be used and distributed according to the terms of the
 * 3-Clause BSD License
 *
 * Hand-written debug instructions whose convention is to return the input id
 * (target / type) rather than the result of `<< EndOp{}`. The simple
 * generator template can't express that. OpString and OpLine live in
 * _generated.cpp.
 */

#include "sirit/sirit.h"

#include "common_types.h"
#include "stream.h"

namespace Sirit {

Id Module::Name(Id target, std::string_view name) {
    debug->Reserve(3 + WordsInString(name));
    *debug << spv::Op::OpName << target << name << EndOp{};
    return target;
}

Id Module::MemberName(Id type, u32 member, std::string_view name) {
    debug->Reserve(4 + WordsInString(name));
    *debug << spv::Op::OpMemberName << type << member << name << EndOp{};
    return type;
}

} // namespace Sirit
