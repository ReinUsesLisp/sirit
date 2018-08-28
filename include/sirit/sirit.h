/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
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
class Operand;

typedef const Op* Ref;

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
    void AddEntryPoint(spv::ExecutionModel execution_model, Ref entry_point,
                       const std::string& name, const std::vector<Ref>& interfaces = {});

    /**
     * Adds an instruction to module's code
     * @param op Instruction to insert into code. Types and constants must not be emitted.
     * @return Returns op.
     */
    Ref Emit(Ref op);
 
    // Types

    /// Returns type void.
    Ref TypeVoid();

    /// Returns type bool.
    Ref TypeBool();

    /// Returns type integer.
    Ref TypeInt(int width, bool is_signed);

    /// Returns type float.
    Ref TypeFloat(int width);

    /// Returns type vector.
    Ref TypeVector(Ref component_type, int component_count);

    /// Returns type matrix.
    Ref TypeMatrix(Ref column_type, int column_count);

    /// Returns type image.
    Ref TypeImage(Ref sampled_type, spv::Dim dim, int depth, bool arrayed, bool ms,
                        int sampled, spv::ImageFormat image_format,
                        spv::AccessQualifier access_qualifier = static_cast<spv::AccessQualifier>(Undefined));

    /// Returns type sampler.
    Ref TypeSampler();

    /// Returns type sampled image.
    Ref TypeSampledImage(Ref image_type);

    /// Returns type array.
    Ref TypeArray(Ref element_type, Ref length);

    /// Returns type runtime array.
    Ref TypeRuntimeArray(Ref element_type);

    /// Returns type struct.
    Ref TypeStruct(const std::vector<Ref>& members = {});

    /// Returns type opaque.
    Ref TypeOpaque(const std::string& name);

    /// Returns type pointer.
    Ref TypePointer(spv::StorageClass storage_class, Ref type);

    /// Returns type function.
    Ref TypeFunction(Ref return_type, const std::vector<Ref>& arguments = {});

    /// Returns type event.
    Ref TypeEvent();

    /// Returns type device event.
    Ref TypeDeviceEvent();

    /// Returns type reserve id.
    Ref TypeReserveId();

    /// Returns type queue.
    Ref TypeQueue();

    /// Returns type pipe.
    Ref TypePipe(spv::AccessQualifier access_qualifier);

    // Constant
    
    /// Returns a true scalar constant.
    Ref ConstantTrue(Ref result_type);

    /// Returns a false scalar constant.
    Ref ConstantFalse(Ref result_type);

    /// Returns a numeric scalar constant.
    Ref Constant(Ref result_type, Operand* literal);

    /// Returns a numeric scalar constant.
    Ref ConstantComposite(Ref result_type, const std::vector<Ref>& constituents);

    // Function

    /// Emits a function.
    Ref Function(Ref result_type, spv::FunctionControlMask function_control, Ref function_type);

    /// Emits a function end.
    Ref FunctionEnd();

    // Flow

    /// Emits a label. It starts a block.
    Ref Label();

    /// Emits a return. It ends a block.
    Ref Return();

    // Literals
    static Operand* Literal(std::uint32_t value);
    static Operand* Literal(std::uint64_t value);
    static Operand* Literal(std::int32_t value);
    static Operand* Literal(std::int64_t value);
    static Operand* Literal(float value);
    static Operand* Literal(double value);

private:
    Ref AddCode(Op* op);

    Ref AddCode(spv::Op opcode, std::uint32_t id = UINT32_MAX);

    Ref AddDeclaration(Op* op);

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

    std::vector<Ref> code;

    std::vector<std::unique_ptr<Op>> code_store;
};

} // namespace Sirit
