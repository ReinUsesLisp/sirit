add_library(sirit
    ../include/sirit/sirit.h
    sirit.cpp
    op.cpp
    op.h
    stream.cpp
    stream.h
    operand.cpp
    operand.h
    literal-number.cpp
    literal-number.h
    literal-string.cpp
    literal-string.h
    common_types.h
    insts.h
    insts/type.cpp
    insts/constant.cpp
    insts/function.cpp
    insts/flow.cpp
    insts/debug.cpp
    insts/memory.cpp
    insts/annotation.cpp
)
target_include_directories(sirit
                           PUBLIC ../include
                           PRIVATE . ${SPIRV-Headers_SOURCE_DIR}/include
                           INTERFACE ${SPIRV-Headers_SOURCE_DIR}/include)
