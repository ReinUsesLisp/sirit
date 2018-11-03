/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "op.h"
#include "sirit/sirit.h"
#include <memory>
#include <string>

namespace Sirit {

Id Module::Name(Id target, const std::string& name) {
    auto op{std::make_unique<Op>(spv::Op::OpName)};
    op->Add(target);
    op->Add(name);
    debug.push_back(std::move(op));
    return target;
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
