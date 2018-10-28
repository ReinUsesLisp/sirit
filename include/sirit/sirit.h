/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <spirv/unified1/spirv.hpp11>
#include <vector>

namespace Sirit {

constexpr std::uint32_t GENERATOR_MAGIC_NUMBER = 0;

class Op;
class Operand;

typedef const Op* Ref;

class Module {
  public:
    explicit Module();
    ~Module();

    /**
     * Assembles current module into a SPIR-V stream.
     * It can be called multiple times but it's recommended to copy code
     * externally.
     * @return A stream of bytes representing a SPIR-V module.
     */
    std::vector<std::uint8_t> Assemble() const;

    /// Adds a module capability.
    void AddCapability(spv::Capability capability);

    /// Sets module memory model.
    void SetMemoryModel(spv::AddressingModel addressing_model,
                        spv::MemoryModel memory_model);

    /// Adds an entry point.
    void AddEntryPoint(spv::ExecutionModel execution_model, Ref entry_point,
                       const std::string& name,
                       const std::vector<Ref>& interfaces = {});

    /**
     * Adds an instruction to module's code
     * @param op Instruction to insert into code. Types and constants must not
     * be emitted.
     * @return Returns op.
     */
    Ref Emit(Ref op);

    /**
     * Adds a global variable
     * @param variable Global variable to add.
     * @return Returns variable.
     */
    Ref AddGlobalVariable(Ref variable);

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
    Ref TypeImage(Ref sampled_type, spv::Dim dim, int depth, bool arrayed,
                  bool ms, int sampled, spv::ImageFormat image_format,
                  std::optional<spv::AccessQualifier> access_qualifier = {});

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
    Ref ConstantComposite(Ref result_type,
                          const std::vector<Ref>& constituents);

    /// Returns a sampler constant.
    Ref ConstantSampler(Ref result_type,
                        spv::SamplerAddressingMode addressing_mode,
                        bool normalized, spv::SamplerFilterMode filter_mode);

    /// Returns a null constant value.
    Ref ConstantNull(Ref result_type);

    // Function

    /// Declares a function.
    Ref Function(Ref result_type, spv::FunctionControlMask function_control,
                 Ref function_type);

    /// Ends a function.
    Ref FunctionEnd();

    // Flow

    /// Declare a structured loop.
    Ref LoopMerge(Ref merge_block, Ref continue_target,
                  spv::LoopControlMask loop_control,
                  const std::vector<Ref>& literals = {});

    /// Declare a structured selection.
    Ref SelectionMerge(Ref merge_block,
                       spv::SelectionControlMask selection_control);

    /// The block label instruction: Any reference to a block is through this
    /// ref.
    Ref Label();

    /// Unconditional jump to label.
    Ref Branch(Ref target_label);

    /// If condition is true branch to true_label, otherwise branch to
    /// false_label.
    Ref BranchConditional(Ref condition, Ref true_label, Ref false_label,
                          std::uint32_t true_weight = 0,
                          std::uint32_t false_weight = 0);

    /// Returns with no value from a function with void return type.
    Ref Return();

    // Debug

    /// Assign a name string to a reference.
    /// @return target
    Ref Name(Ref target, const std::string& name);

    // Memory

    /// Allocate an object in memory, resulting in a copy to it.
    Ref Variable(Ref result_type, spv::StorageClass storage_class,
                 Ref initializer = nullptr);

    // Annotation

    /// Add a decoration to target.
    Ref Decorate(Ref target, spv::Decoration decoration,
                 const std::vector<Operand*>& literals = {});

    Ref MemberDecorate(Ref structure_type, Operand* member, spv::Decoration decoration,
            const std::vector<Operand*>& literals = {});

    // Literals
    static Operand* Literal(std::uint32_t value);
    static Operand* Literal(std::uint64_t value);
    static Operand* Literal(std::int32_t value);
    static Operand* Literal(std::int64_t value);
    static Operand* Literal(float value);
    static Operand* Literal(double value);

  private:
    Ref AddCode(Op* op);

    Ref AddCode(spv::Op opcode, std::optional<std::uint32_t> id = {});

    Ref AddDeclaration(Op* op);

    Ref AddAnnotation(Op* op);

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

    std::vector<Ref> global_variables;

    std::vector<Ref> code;

    std::vector<std::unique_ptr<Op>> code_store;
};

} // namespace Sirit
