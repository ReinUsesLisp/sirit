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

static const std::uint32_t Undefined = UINT32_MAX;

class Op;

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
    void AddEntryPoint(spv::ExecutionModel execution_model, const Op* entry_point,
                       const std::string& name, const std::vector<const Op*>& interfaces = {});

    /**
     * Adds an instruction to module's code
     * @param op Instruction to insert into code. Types and constants must not be emitted.
     * @return Returns op.
     */
    const Op* Emit(const Op* op);

    // Types

    /// Returns type void.
    const Op* TypeVoid();

    /// Returns type bool.
    const Op* TypeBool();

    /// Returns type integer.
    const Op* TypeInt(int width, bool is_signed);

    /// Returns type float.
    const Op* TypeFloat(int width);

    /// Returns type vector.
    const Op* TypeVector(const Op* component_type, int component_count);

    /// Returns type matrix.
    const Op* TypeMatrix(const Op* column_type, int column_count);

    /// Returns type image.
    const Op* TypeImage(const Op* sampled_type, spv::Dim dim, int depth, bool arrayed, bool ms,
                        int sampled, spv::ImageFormat image_format,
                        spv::AccessQualifier access_qualifier = static_cast<spv::AccessQualifier>(Undefined));

    /// Returns type sampler.
    const Op* TypeSampler();

    /// Returns type sampled image.
    const Op* TypeSampledImage(const Op* image_type);

    /// Returns type array.
    const Op* TypeArray(const Op* element_type, const Op* length);

    /// Returns type runtime array.
    const Op* TypeRuntimeArray(const Op* element_type);

    /// Returns type struct.
    const Op* TypeStruct(const std::vector<const Op*>& members = {});

    /// Returns type opaque.
    const Op* TypeOpaque(const std::string& name);

    /// Returns type pointer.
    const Op* TypePointer(spv::StorageClass storage_class, const Op* type);

    /// Returns type function.
    const Op* TypeFunction(const Op* return_type, const std::vector<const Op*>& arguments = {});

    /// Returns type event.
    const Op* TypeEvent();

    /// Returns type device event.
    const Op* TypeDeviceEvent();

    /// Returns type reserve id.
    const Op* TypeReserveId();

    /// Returns type queue.
    const Op* TypeQueue();

    /// Returns type pipe.
    const Op* TypePipe(spv::AccessQualifier access_qualifier);

    // Function

    /// Emits a function.
    const Op* Function(const Op* result_type, spv::FunctionControlMask function_control,
                       const Op* function_type);

    /// Emits a function end.
    const Op* FunctionEnd();

    // Flow

    /// Emits a label. It starts a block.
    const Op* Label();

    /// Emits a return. It ends a block.
    const Op* Return();

private:
    const Op* AddCode(Op* op);

    const Op* AddCode(spv::Op opcode, std::uint32_t id = UINT32_MAX);

    const Op* AddDeclaration(Op* op);

    std::uint32_t bound{1};

    std::set<spv::Capability> capabilities;

    std::set<std::string> extensions;

    std::set<std::unique_ptr<Op>> ext_inst_import;

    spv::AddressingModel addressing_model{spv::AddressingModel::Logical};
    spv::MemoryModel memory_model{spv::MemoryModel::GLSL450};

    std::vector<std::unique_ptr<Op>> entry_points;

    std::vector<std::unique_ptr<Op>> execution_mode;

    std::vector<std::unique_ptr<Op>> debug;

    std::vector<std::unique_ptr<Op>> annotations;

    std::vector<std::unique_ptr<Op>> declarations;

    std::vector<const Op*> code;

    std::vector<std::unique_ptr<Op>> code_store;
};

} // namespace Sirit
