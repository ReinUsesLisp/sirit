add_library(${PROJECT_NAME}
    sirit.cpp
    op.cpp
    stream.cpp
    operand.cpp
    literal_number.cpp
    literal_string.cpp
    instructions/type.cpp
    instructions/constant.cpp
    instructions/function.cpp
    instructions/flow.cpp
    instructions/debug.cpp
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
    instructions/atomic.cpp)

if(MSVC)
target_compile_options(sirit PRIVATE ${SIRIT_CXX_FLAGS})
endif(MSVC)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
    PUBLIC 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    INTERFACE 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(
    ${PROJECT_NAME}
    SPIRV-Tools)


