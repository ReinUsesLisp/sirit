/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include <algorithm>
#include <cassert>
#include "sirit/sirit.h"
#include "common_types.h"
#include "ref.h"
#include "stream.h"

namespace Sirit {

template<typename T>
static void WriteEnum(Stream& stream, spv::Op op, T value) {
    Ref ref{op};
    ref.Add(static_cast<u32>(value));
    ref.Write(stream);
}

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

    Ref memory_model_ref{spv::Op::OpMemoryModel};
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

void Module::AddEntryPoint(spv::ExecutionModel execution_model, const Ref* entry_point,
                           const std::string& name, const std::vector<const Ref*>& interfaces) {
    Ref* op{new Ref(spv::Op::OpEntryPoint)};
    op->Add(static_cast<u32>(execution_model));
    op->Add(entry_point);
    op->Add(name);
    op->Add(interfaces);
    entry_points.push_back(std::unique_ptr<Ref>(op));
}

const Ref* Module::TypeVoid() {
    return AddDeclaration(new Ref(spv::Op::OpTypeVoid, bound));
}

const Ref* Module::TypeFunction(const Ref* return_type, const std::vector<const Ref*>& arguments) {
    Ref* type_func{new Ref(spv::Op::OpTypeFunction, bound)};
    type_func->Add(return_type);
    for (const Ref* arg : arguments) {
        type_func->Add(arg);
    }
    return AddDeclaration(type_func);
}

void Module::Add(const Ref* ref) {
    assert(ref);
    code.push_back(ref);
}

const Ref* Module::EmitFunction(const Ref* result_type, spv::FunctionControlMask function_control,
                                const Ref* function_type) {
    Ref* op{new Ref{spv::Op::OpFunction, bound++, result_type}};
    op->Add(static_cast<u32>(function_control));
    op->Add(function_type);
    return AddCode(op);
}

const Ref* Module::EmitLabel() {
    return AddCode(spv::Op::OpLabel, bound++);
}

const Ref* Module::EmitReturn() {
    return AddCode(spv::Op::OpReturn);
}

const Ref* Module::EmitFunctionEnd() {
    return AddCode(spv::Op::OpFunctionEnd);
}

const Ref* Module::AddCode(Ref* ref) {
    code_store.push_back(std::unique_ptr<Ref>(ref));
    return ref;
}

const Ref* Module::AddCode(spv::Op opcode, u32 id) {
    return AddCode(new Ref{opcode, id});
}

const Ref* Module::AddDeclaration(Ref* ref) {
    const auto& found{std::find_if(declarations.begin(), declarations.end(), [=](const auto& other) {
        return *other == *ref;
    })};
    if (found != declarations.end()) {
        delete ref;
        return found->get();
    } else {
        declarations.push_back(std::unique_ptr<Ref>(ref));
        bound++;
        return ref;
    }
}

} // namespace Sirit
