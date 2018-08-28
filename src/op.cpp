/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <cassert>
#include "common_types.h"
#include "operand.h"
#include "op.h"
#include "lnumber.h"
#include "lstring.h"

namespace Sirit {

Op::Op(spv::Op opcode_, u32 id_, Ref result_type_) 
    : opcode(opcode_), id(id_), result_type(result_type_) {
    operand_type = OperandType::Op;
}

Op::~Op() = default;

void Op::Fetch(Stream& stream) const {
    assert(id != UINT32_MAX);
    stream.Write(id);
}

u16 Op::GetWordCount() const {
    return 1;
}

bool Op::operator==(const Operand& other) const {
    if (operand_type != other.GetType()) {
        return false;
    }
    const Op& op = dynamic_cast<const Op&>(other);
    if (op.opcode == opcode && result_type == op.result_type &&
        operands.size() == op.operands.size()) {
        for (std::size_t i{}; i < operands.size(); i++) {
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
    if (id != UINT32_MAX) {
        stream.Write(id);
    }
    for (const Operand* operand : operands) {
        operand->Fetch(stream);
    }
}

void Op::Add(Operand* operand) {
    Add(static_cast<const Operand*>(operand));
    operand_store.push_back(std::unique_ptr<Operand>(operand));
}

void Op::Add(const Operand* operand) {
    operands.push_back(operand);
}

void Op::Add(u32 integer) {
    Add(new LiteralNumber(integer));
}

void Op::Add(const std::string& string) {
    Add(new LiteralString(string));
}

void Op::Add(const std::vector<Ref>& ids) {
    for (Ref op : ids) {
        Add(op);
    }
}

u16 Op::WordCount() const {
    u16 count{1};
    if (result_type) {
        count++;
    }
    if (id != UINT32_MAX) {
        count++;
    }
    for (const Operand* operand : operands) {
        count += operand->GetWordCount();
    }
    return count;
}

} // namespace Sirit
