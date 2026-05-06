# Tests pull in two extra dependencies via FetchContent so that consumers of
# the library never need them: SPIRV-Tools (for spirv-val + disassembly) and
# SPIRV-Cross (for round-trip cross-compilation back to GLSL/HLSL). The test
# target uses these to verify the SPIR-V we generate is valid for ray tracing
# and cooperative matrix workloads.

include(FetchContent)

# ---- SPIRV-Tools --------------------------------------------------------
# Reuse the SPIRV-Headers we already have at externals/SPIRV-Headers, so
# SPIRV-Tools doesn't fetch a duplicate copy.
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SPIRV_WERROR OFF CACHE BOOL "" FORCE)
set(SPIRV_BUILD_FUZZER OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Pin to main; any SPIRV-Tools release older than the SPIRV-Headers we bundle
# will fail to compile because the auto-generated tables reference operand
# kinds (CooperativeVector, Tensor, MatMul) that older SPIRV-Tools doesn't
# declare. main tracks SPIRV-Headers closely.
FetchContent_Declare(
    SPIRV-Tools
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SPIRV-Tools)

# ---- SPIRV-Cross --------------------------------------------------------
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_HLSL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_SKIP_INSTALL ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    SPIRV-Cross
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
    GIT_TAG        vulkan-sdk-1.3.290.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SPIRV-Cross)

# ---- sirit_tests --------------------------------------------------------
add_executable(sirit_tests
    main.cpp
    test_helpers.h
    test_helpers.cpp
    test_cross.h
    test_cross.cpp
    test_cooperative_matrix.cpp
    test_raytracing.cpp
    test_vertex_shader_replicated.cpp
    test_fragment_shader_replicated.cpp
    test_compute_shader_replicated.cpp
)
target_link_libraries(sirit_tests PRIVATE
    sirit
    SPIRV-Tools-static
    spirv-cross-glsl
    spirv-cross-hlsl
    spirv-cross-core
)
target_include_directories(sirit_tests PRIVATE . ../include)

# Ensure the tests find SPIRV-Tools headers (FetchContent_MakeAvailable
# doesn't propagate the include directory through the static target on all
# CMake versions).
target_include_directories(sirit_tests PRIVATE
    ${spirv-tools_SOURCE_DIR}/include
)

add_test(sirit_tests sirit_tests)
