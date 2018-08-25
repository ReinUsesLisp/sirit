/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include <cassert>
#include "common_types.h"
#include "operand.h"
#include "ref.h"

namespace Sirit {

Ref::Ref(spv::Op opcode_, u32 id_, const Ref* result_type_) 
    : opcode(opcode_), id(id_), result_type(result_type_) {
    operand_type = OperandType::Ref;
}

Ref::~Ref() = default;

void Ref::Fetch(Stream& stream) const {
    assert(id != UINT32_MAX);
    stream.Write(id);
}

u16 Ref::GetWordCount() const {
    return 1;
}

bool Ref::operator==(const Operand& other) const {
    if (operand_type != other.GetType()) {
        return false;
    }
    const Ref& ref = dynamic_cast<const Ref&>(other);
    if (ref.opcode == opcode && result_type == ref.result_type &&
        operands.size() == ref.operands.size()) {
        for (std::size_t i{}; i < operands.size(); i++) {
            if (*operands[i] != *ref.operands[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void Ref::Write(Stream& stream) const {
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

void Ref::Add(Operand* operand) {
    Add(static_cast<const Operand*>(operand));
    operand_store.push_back(std::unique_ptr<Operand>(operand));
}

void Ref::Add(const Operand* operand) {
    operands.push_back(operand);
}

void Ref::Add(u32 integer) {
    Add(new LiteralInteger(integer));
}

void Ref::Add(const std::string& string) {
    Add(new LiteralString(string));
}

void Ref::Add(const std::vector<const Ref*>& ids) {
    for (const Ref* ref : ids) {
        Add(ref);
    }
}

u16 Ref::WordCount() const {
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
