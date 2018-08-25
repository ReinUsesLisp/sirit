/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include <memory>
#include <spirv/unified1/spirv.hpp11>

namespace Sirit {

static const std::uint32_t GeneratorMagicNumber = 0;

class Ref;

class Module {
public:
    explicit Module();
    ~Module();

    /**
     * Assembles current module into a SPIR-V stream.
     * It can be called multiple times but it's recommended to copy code externally.
     * @return A stream of bytes representing a SPIR-V module.
     */
    std::vector<std::uint8_t> Assembly() const;

    /**
     * Optimizes module's IR.
     * All returned references become invalid.
     * @param level Level of optimization.
     */
    void Optimize(int level);

    /// Adds a module capability.
    void AddCapability(spv::Capability capability);

    /// Sets module memory model.
    void SetMemoryModel(spv::AddressingModel addressing_model, spv::MemoryModel memory_model);

    /// Adds an entry point.
    void AddEntryPoint(spv::ExecutionModel execution_model, const Ref* entry_point,
                       const std::string& name, const std::vector<const Ref*>& interfaces = {});

    /// Returns type void.
    const Ref* TypeVoid();

    /// Returns a function type.
    const Ref* TypeFunction(const Ref* return_type, const std::vector<const Ref*>& arguments = {});

    /// Adds a reference to code block
    void Add(const Ref* ref);

    /// Emits a function.
    const Ref* EmitFunction(const Ref* result_type, spv::FunctionControlMask function_control,
                            const Ref* function_type);

    /// Emits a label. It starts a block.
    const Ref* EmitLabel();

    /// Emits a return. It ends a block.
    const Ref* EmitReturn();

    /// Emits a function end.
    const Ref* EmitFunctionEnd();

private:
    const Ref* AddCode(Ref* ref);

    const Ref* AddCode(spv::Op opcode, std::uint32_t id = UINT32_MAX);

    const Ref* AddDeclaration(Ref* ref);

    std::uint32_t bound{1};

    std::set<spv::Capability> capabilities;

    std::set<std::string> extensions;

    std::set<std::unique_ptr<Ref>> ext_inst_import;

    spv::AddressingModel addressing_model{spv::AddressingModel::Logical};
    spv::MemoryModel memory_model{spv::MemoryModel::GLSL450};

    std::vector<std::unique_ptr<Ref>> entry_points;

    std::vector<std::unique_ptr<Ref>> execution_mode;

    std::vector<std::unique_ptr<Ref>> debug;

    std::vector<std::unique_ptr<Ref>> annotations;

    std::vector<std::unique_ptr<Ref>> declarations;

    std::vector<const Ref*> code;

    std::vector<std::unique_ptr<Ref>> code_store;
};

} // namespace Sirit
