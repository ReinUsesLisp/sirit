Sirit
=====
A runtime SPIR-V assembler. It aims to ease dynamic SPIR-V code generation
without calling external applications (like Khronos' `spirv-as`)  

Its design aims to move code that does not belong to the application in the
library without, limitting its functionality.

What it does for you:
* Sort declaration opcodes
* Handle types and constant duplicates
* Emit SPIR-V opcodes  

What it will try to do in the future:
* Add capabilities automatically
* Optimize code

What does not do for you:
* Avoid ID duplicates (emitting the same instruction twice)
* Dump code to disk
* Handle code blocks/branches
* Compile from a higher level language
  
  
It's in early stages of development, many instructions are missing since
they are written manually instead of being generated from a file.

Example
-------

```cpp
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

// Then...

MyModule module;
module.Generate();

module.Optimize(2);
std::vector<std::uint8_t> code{module.Assemble()};
```
