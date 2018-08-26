/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include <algorithm>
#include <cassert>
#include "sirit/sirit.h"
#include "common_types.h"
#include "op.h"
#include "stream.h"
#include "opcodes.h"

namespace Sirit {

Module::Module() {}

Module::~Module() = default;

std::vector<u8> Module::Assembly() const {
    std::vector<u8> bytes;
    Stream stream{bytes};

    stream.Write(spv::MagicNumber);
    stream.Write(spv::Version);
    stream.Write(GeneratorMagicNumber);
    stream.Write(bound);
    stream.Write(static_cast<u32>(0));

    for (auto capability : capabilities) {
        WriteEnum(stream, spv::Op::OpCapability, capability);
    }

    // TODO write extensions

    // TODO write ext inst imports

    Op memory_model_ref{spv::Op::OpMemoryModel};
    memory_model_ref.Add(static_cast<u32>(addressing_model));
    memory_model_ref.Add(static_cast<u32>(memory_model));
    memory_model_ref.Write(stream);

    for (const auto& entry_point : entry_points) {
        entry_point->Write(stream);
    }

    // TODO write execution mode

    // TODO write debug symbols

    // TODO write annotations

    for (const auto& decl : declarations) {
        decl->Write(stream);
    }
    for (const auto& line : code) {
        line->Write(stream);
    }

    return bytes;
}

void Module::Optimize(int level) {
}

void Module::AddCapability(spv::Capability capability) {
    capabilities.insert(capability);
}

void Module::SetMemoryModel(spv::AddressingModel addressing_model, spv::MemoryModel memory_model) {
    this->addressing_model = addressing_model;
    this->memory_model = memory_model;
}

void Module::AddEntryPoint(spv::ExecutionModel execution_model, const Op* entry_point,
                           const std::string& name, const std::vector<const Op*>& interfaces) {
    Op* op{new Op(spv::Op::OpEntryPoint)};
    op->Add(static_cast<u32>(execution_model));
    op->Add(entry_point);
    op->Add(name);
    op->Add(interfaces);
    entry_points.push_back(std::unique_ptr<Op>(op));
}

const Op* Module::Emit(const Op* op) {
    assert(op);
    code.push_back(op);
    return op;
}

const Op* Module::Function(const Op* result_type, spv::FunctionControlMask function_control,
                            const Op* function_type) {
    Op* op{new Op{spv::Op::OpFunction, bound++, result_type}};
    op->Add(static_cast<u32>(function_control));
    op->Add(function_type);
    return AddCode(op);
}

const Op* Module::Label() {
    return AddCode(spv::Op::OpLabel, bound++);
}

const Op* Module::Return() {
    return AddCode(spv::Op::OpReturn);
}

const Op* Module::FunctionEnd() {
    return AddCode(spv::Op::OpFunctionEnd);
}

const Op* Module::AddCode(Op* op) {
    code_store.push_back(std::unique_ptr<Op>(op));
    return op;
}

const Op* Module::AddCode(spv::Op opcode, u32 id) {
    return AddCode(new Op{opcode, id});
}

const Op* Module::AddDeclaration(Op* op) {
    const auto& found{std::find_if(declarations.begin(), declarations.end(), [=](const auto& other) {
        return *other == *op;
    })};
    if (found != declarations.end()) {
        delete op;
        return found->get();
    } else {
        declarations.push_back(std::unique_ptr<Op>(op));
        bound++;
        return op;
    }
}

} // namespace Sirit
