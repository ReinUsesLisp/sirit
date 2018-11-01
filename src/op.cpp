/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <cassert>

#include "common_types.h"
#include "literal_number.h"
#include "literal_string.h"
#include "op.h"
#include "operand.h"

namespace Sirit {

Op::Op(spv::Op opcode, std::optional<u32> id, Id result_type)
    : opcode(opcode), id(id), result_type(result_type) {
    operand_type = OperandType::Op;
}

Op::~Op() = default;

void Op::Fetch(Stream& stream) const {
    assert(id.has_value());
    stream.Write(id.value());
}

u16 Op::GetWordCount() const { return 1; }

bool Op::operator==(const Operand& other) const {
    if (operand_type != other.GetType()) {
        return false;
    }
    const Op& op = dynamic_cast<const Op&>(other);
    if (op.opcode == opcode && result_type == op.result_type &&
        operands.size() == op.operands.size()) {
        for (std::size_t i = 0; i < operands.size(); i++) {
            if (*operands[i] != *op.operands[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void Op::Write(Stream& stream) const {
    stream.Write(static_cast<u16>(opcode));
    stream.Write(WordCount());

    if (result_type) {
        result_type->Fetch(stream);
    }
    if (id.has_value()) {
        stream.Write(id.value());
    }
    for (const auto* operand : operands) {
        operand->Fetch(stream);
    }
}

void Op::Sink(Operand* operand) {
    Add(static_cast<const Operand*>(operand));
    operand_store.push_back(std::unique_ptr<Operand>(operand));
}

void Op::Sink(const std::vector<Operand*>& operands) {
    for (auto* operand : operands) {
        Sink(operand);
    }
}

void Op::Add(const Literal& literal) {
    Operand* operand = [&]() {
        switch (literal.index()) {
        case 0:
            return LiteralNumber::Create(std::get<0>(literal));
        case 1:
            return LiteralNumber::Create(std::get<1>(literal));
        case 2:
            return LiteralNumber::Create(std::get<2>(literal));
        case 3:
            return LiteralNumber::Create(std::get<3>(literal));
        case 4:
            return LiteralNumber::Create(std::get<4>(literal));
        case 5:
            return LiteralNumber::Create(std::get<5>(literal));
        default:
            assert(!"invalid literal type");
            abort();
        }
    }();
    Sink(operand);
}

void Op::Add(const std::vector<Literal>& literals) {
    for (const auto& literal : literals) {
        Add(literal);
    }
}

void Op::Add(const Operand* operand) { operands.push_back(operand); }

void Op::Add(u32 integer) { Sink(LiteralNumber::Create<u32>(integer)); }

void Op::Add(const std::string& string) { Sink(new LiteralString(string)); }

void Op::Add(const std::vector<Id>& ids) {
    for (Id op : ids) {
        Add(op);
    }
}

u16 Op::WordCount() const {
    u16 count = 1;
    if (result_type) {
        count++;
    }
    if (id.has_value()) {
        count++;
    }
    for (const Operand* operand : operands) {
        count += operand->GetWordCount();
    }
    return count;
}

} // namespace Sirit
