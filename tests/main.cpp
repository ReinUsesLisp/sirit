/* This file is part of the sirit project.
 * Copyright (c) 2018 ReinUsesLisp
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
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
        
        auto main_type{TypeFunction(TypeVoid())};
        auto main_func{Emit(Function(TypeVoid(), spv::FunctionControlMask::MaskNone, main_type))};
        Emit(Label());
        Emit(Return());
        Emit(FunctionEnd());

        AddEntryPoint(spv::ExecutionModel::Vertex, main_func, "main");
    }
};

int main(int argc, char** argv) {
    MyModule module;
    module.Generate();

    module.Optimize(2);
    std::vector<std::uint8_t> code{module.Assembly()};

    FILE* file = fopen("sirit.spv", "wb");
    fwrite(code.data(), 1, code.size(), file);
    fclose(file);

    return 0;
}
