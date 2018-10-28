/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include "sirit/sirit.h"
#include "common_types.h"
#include "op.h"
#include "stream.h"
#include <algorithm>
#include <cassert>

namespace Sirit {

template <typename T>
static void WriteEnum(Stream& stream, spv::Op opcode, T value) {
    Op op{opcode};
    op.Add(static_cast<u32>(value));
    op.Write(stream);
}

template <typename T>
static void WriteSet(Stream& stream, const T& set) {
    for (const auto& item : set) {
        item->Write(stream);
    }
}

Module::Module() {}

Module::~Module() = default;

std::vector<u8> Module::Assemble() const {
    std::vector<u8> bytes;
    Stream stream{bytes};

    stream.Write(spv::MagicNumber);
    stream.Write(spv::Version);
    stream.Write(GENERATOR_MAGIC_NUMBER);
    stream.Write(bound);
    stream.Write(static_cast<u32>(0));

    for (const auto capability : capabilities) {
        WriteEnum(stream, spv::Op::OpCapability, capability);
    }
    // TODO write extensions
    // TODO write ext inst imports

    Op memory_model_ref{spv::Op::OpMemoryModel};
    memory_model_ref.Add(static_cast<u32>(addressing_model));
    memory_model_ref.Add(static_cast<u32>(memory_model));
    memory_model_ref.Write(stream);

    WriteSet(stream, entry_points);
    // TODO write execution mode
    WriteSet(stream, debug);
    WriteSet(stream, annotations);
    WriteSet(stream, declarations);
    WriteSet(stream, global_variables);
    WriteSet(stream, code);

    return bytes;
}

void Module::AddCapability(spv::Capability capability) {
    capabilities.insert(capability);
}

void Module::SetMemoryModel(spv::AddressingModel addressing_model,
                            spv::MemoryModel memory_model) {
    this->addressing_model = addressing_model;
    this->memory_model = memory_model;
}

void Module::AddEntryPoint(spv::ExecutionModel execution_model, Ref entry_point,
                           const std::string& name,
                           const std::vector<Ref>& interfaces) {
    auto const op{new Op(spv::Op::OpEntryPoint)};
    op->Add(static_cast<u32>(execution_model));
    op->Add(entry_point);
    op->Add(name);
    op->Add(interfaces);
    entry_points.push_back(std::unique_ptr<Op>(op));
}

Ref Module::Emit(Ref op) {
    assert(op);
    code.push_back(op);
    return op;
}

Ref Module::AddGlobalVariable(Ref variable) {
    assert(variable);
    global_variables.push_back(variable);
    return variable;
}

Ref Module::AddCode(Op* op) {
    assert(op);
    code_store.push_back(std::unique_ptr<Op>(op));
    return op;
}

Ref Module::AddCode(spv::Op opcode, std::optional<u32> id) {
    return AddCode(new Op(opcode, id));
}

Ref Module::AddDeclaration(Op* op) {
    const auto& found{
        std::find_if(declarations.begin(), declarations.end(),
                     [&op](const auto& other) { return *other == *op; })};
    if (found != declarations.end()) {
        delete op;
        return found->get();
    } else {
        declarations.push_back(std::unique_ptr<Op>(op));
        bound++;
        return op;
    }
}

Ref Module::AddAnnotation(Op* op) {
    assert(op);
    annotations.push_back(std::unique_ptr<Op>(op));
    return op;
}

} // namespace Sirit
