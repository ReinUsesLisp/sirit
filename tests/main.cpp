/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * Lesser General Public License version 2.1 or any later version.
 */

#include <sirit/sirit.h>
#include <cstdio>
#include <cstdlib>

class MyModule : public Sirit::Module {
public:
    MyModule() {}
    ~MyModule() = default;

    void Generate() {
        AddCapability(spv::Capability::Shader);
        SetMemoryModel(spv::AddressingModel::Logical, spv::MemoryModel::GLSL450);
        
        // Type testing
        TypeBool();
        TypeBool();
        TypeInt(64, false);
        TypeInt(16, false);
        TypeFloat(16);
        TypeFloat(32);
        TypeFloat(64);
        TypeVector(TypeBool(), 4);
        TypeVector(TypeBool(), 3);
        TypeMatrix(TypeVector(TypeFloat(32), 4), 4);
        TypeImage(TypeFloat(32), spv::Dim::Dim2D, 0, false, false, 0,
                  spv::ImageFormat::Rg32f);
        TypeSampledImage(TypeImage(TypeFloat(32), spv::Dim::Rect, 0, false, false, 0,
                                   spv::ImageFormat::Rg32f));
        TypeVector(TypeInt(32, false), 4);
        TypeVector(TypeInt(64, false), 4);
        TypeRuntimeArray(TypeInt(32, false));
        TypeStruct({TypeInt(32, false), TypeFloat(64)});
        TypePointer(spv::StorageClass::Private, TypeFloat(16));
        ConstantTrue(TypeBool());
        ConstantTrue(TypeBool());
        ConstantFalse(TypeBool());
        Constant(TypeFloat(64), Literal(6342.21));
        Constant(TypeFloat(32), Literal(6342.21f));
        Constant(TypeFloat(16), Literal(30u));
        Constant(TypeInt(32, false), Literal(30u));
        Constant(TypeInt(16, false), Literal(30u));
        Constant(TypeInt(8, false), Literal(30u));
        ConstantComposite(TypeVector(TypeFloat(32), 2),
                          {Constant(TypeFloat(32), Literal(50.0f)),
                           Constant(TypeFloat(32), Literal(50.0f))});
        ConstantNull(TypeVector(TypeInt(64, false), 4));

        auto cont{Name(Label(), "cont")};
        auto skip{Name(Label(), "skip")};
        auto main_type{TypeFunction(TypeVoid())};
        auto main_func{Emit(Function(TypeVoid(), spv::FunctionControlMask::MaskNone, main_type))};
        Emit(Label());
        Emit(BranchConditional(ConstantTrue(TypeBool()), cont, skip, 5, 0));
        Emit(cont);
        Emit(Branch(skip));
        Emit(skip);
        Emit(Return());
        Emit(FunctionEnd());

        AddEntryPoint(spv::ExecutionModel::Vertex, main_func, "main");
    }
};

int main(int argc, char** argv) {
    MyModule module;
    module.Generate();

    module.Optimize(2);
    std::vector<std::uint8_t> code{module.Assemble()};

    FILE* file = fopen("sirit.spv", "wb");
    fwrite(code.data(), 1, code.size(), file);
    fclose(file);

    return 0;
}
