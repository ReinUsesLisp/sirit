/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <sirit/sirit.h>
#include <cstdio>
#include <cstdlib>

using u32 = uint32_t;

class MyModule : public Sirit::Module {
public:
    MyModule() = default;
    ~MyModule() = default;

    void Generate() {
        AddCapability(spv::Capability::Shader);
        SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::GLSL450);
        
        const auto t_void = Name(TypeVoid(), "void");
        const auto t_uint = Name(TypeInt(32, false), "uint");
        const auto t_float = Name(TypeFloat(32), "float");

        const auto float4 = Name(TypeVector(t_float, 4), "float4");
        const auto in_float = Name(TypePointer(spv::StorageClass::Input, t_float), "in_float");
        const auto in_float4 = Name(TypePointer(spv::StorageClass::Input, float4), "in_float4");
        const auto out_float4 = Name(TypePointer(spv::StorageClass::Output, float4), "out_float4");

        const auto gl_per_vertex = Name(TypeStruct({float4}), "gl_PerVertex");
        const auto gl_per_vertex_ptr = Name(TypePointer(spv::StorageClass::Output, gl_per_vertex), "out_gl_PerVertex");
        
        const auto in_pos = Name(Variable(in_float4, spv::StorageClass::Input), "in_pos");
        const auto per_vertex = Name(Variable(gl_per_vertex_ptr, spv::StorageClass::Output), "per_vertex");

        Decorate(in_pos, spv::Decoration::Location, {0});
        Decorate(gl_per_vertex, spv::Decoration::Block);
        MemberDecorate(gl_per_vertex, 0, spv::Decoration::BuiltIn, {static_cast<u32>(spv::BuiltIn::Position)});
        
        AddGlobalVariable(in_pos);
        AddGlobalVariable(per_vertex);

        const auto main_func = Emit(Name(Function(t_void, spv::FunctionControlMask::MaskNone, TypeFunction(t_void)), "main"));
        Emit(Label());

        const auto ptr_pos_x = Emit(AccessChain(in_float, in_pos, {Constant(t_uint, 0u)}));
        const auto ptr_pos_y = Emit(AccessChain(in_float, in_pos, {Constant(t_uint, 1u)}));

        const auto pos_x = Emit(Load(t_float, ptr_pos_x));
        const auto pos_y = Emit(Load(t_float, ptr_pos_y));

        auto tmp_position = Emit(Undef(float4));
        tmp_position = Emit(CompositeInsert(float4, pos_x, tmp_position, {0}));
        tmp_position = Emit(CompositeInsert(float4, pos_y, tmp_position, {1}));
        tmp_position = Emit(CompositeInsert(float4, Constant(t_float, 0.f), tmp_position, {2}));
        tmp_position = Emit(CompositeInsert(float4, Constant(t_float, 1.f), tmp_position, {3}));

        const auto gl_position = Emit(AccessChain(out_float4, per_vertex, {Constant(t_uint, 0u)}));
        Emit(Store(gl_position, tmp_position));

        Emit(Return());
        Emit(FunctionEnd());

        AddEntryPoint(spv::ExecutionModel::Vertex, main_func, "main", {in_pos, per_vertex});
    }
};

int main(int argc, char** argv) {
    MyModule module;
    module.Generate();

    std::vector<std::uint8_t> code{module.Assemble()};

    FILE* file = fopen("sirit.spv", "wb");
    fwrite(code.data(), 1, code.size(), file);
    fclose(file);

    return 0;
}
