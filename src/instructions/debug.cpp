/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 3 or any later version.
 */

#include <memory>
#include <string>
#include "op.h"
#include "sirit/sirit.h"

namespace Sirit {

Id Module::Name(Id target, const std::string& name) {
    auto op{std::make_unique<Op>(spv::Op::OpName)};
    op->Add(target);
    op->Add(name);
    debug.push_back(std::move(op));
    return target;
}

Id Module::MemberName(Id type, u32 member, const std::string& name) {
    auto op{std::make_unique<Op>(spv::Op::OpMemberName)};
    op->Add(type);
    op->Add(member);
    op->Add(name);
    debug.push_back(std::move(op));
    return type;
}

Id Module::String(const std::string& string) {
    auto op{std::make_unique<Op>(spv::Op::OpString, bound++)};
    op->Add(string);
    const auto id = op.get();
    debug.push_back(std::move(op));
    return id;
}

Id Module::OpLine(Id file, Literal line, Literal column) {
    auto op{std::make_unique<Op>(spv::Op::OpLine)};
    op->Add(file);
    op->Add(line);
    op->Add(column);
    return AddCode(std::move(op));
}

} // namespace Sirit
