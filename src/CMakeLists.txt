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
    lnumber.cpp
    lnumber.h
    lstring.cpp
    lstring.h
	common_types.h
    insts.h
    insts/type.cpp
    insts/constant.cpp
    insts/function.cpp
    insts/flow.cpp
)
target_include_directories(sirit
                           PUBLIC ../include
                           PRIVATE . ${SPIRV-Headers_SOURCE_DIR}/include
                           INTERFACE ${SPIRV-Headers_SOURCE_DIR}/include)
