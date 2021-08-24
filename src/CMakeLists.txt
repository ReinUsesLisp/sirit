add_library(sirit
    ../include/sirit/sirit.h
    sirit.cpp
    stream.h
    common_types.h
    instructions/type.cpp
    instructions/constant.cpp
    instructions/function.cpp
    instructions/flow.cpp
    instructions/debug.cpp
    instructions/derivatives.cpp
    instructions/memory.cpp
    instructions/annotation.cpp
    instructions/misc.cpp
    instructions/logical.cpp
    instructions/conversion.cpp
    instructions/bit.cpp
    instructions/arithmetic.cpp
    instructions/extension.cpp
    instructions/image.cpp
    instructions/group.cpp
    instructions/barrier.cpp
    instructions/atomic.cpp
)

target_compile_options(sirit PRIVATE ${SIRIT_CXX_FLAGS})

target_include_directories(sirit
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sirit>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sirit PUBLIC SPIRV-Headers)

if (MASTER_PROJECT)
    install(
        DIRECTORY ../include/sirit
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
