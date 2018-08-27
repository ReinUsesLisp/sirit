add_library(sirit
    ../include/sirit/sirit.h
    sirit.cpp
	op.cpp
	op.h
    stream.cpp
    stream.h
    operand.cpp
    operand.h
    literal.cpp
	common_types.h
    opcodes.h
    opcodes/type.cpp
    opcodes/constant.cpp
    opcodes/function.cpp
    opcodes/flow.cpp
)
target_include_directories(sirit
                           PUBLIC ../include
                           PRIVATE . ${SPIRV-Headers_SOURCE_DIR}/include
                           INTERFACE ${SPIRV-Headers_SOURCE_DIR}/include)
