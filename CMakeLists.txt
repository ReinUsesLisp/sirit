# This file has been adapted from dynarmic

cmake_minimum_required(VERSION 3.8)
project(sirit 
        VERSION 
            "0.0.1" 
        DESCRIPTION 
            "A runtime SPIR-V assembler"
        LANGUAGES 
            CXX)

# Default to a Release build
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message(STATUS "Defaulting to a Release build")
endif()

# Set hard requirements for C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warn on CMake API deprecations
set(CMAKE_WARN_DEPRECATED ON)


# Disable in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed.")
endif()

# Add the module directory to the list of paths
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")

# Compiler flags
if (MSVC)
    set(SIRIT_CXX_FLAGS
        /std:c++latest # CMAKE_CXX_STANDARD as no effect on MSVC until CMake 3.10.
        /W4
        /w34263 # Non-virtual member function hides base class virtual function
        /w44265 # Class has virtual functions, but destructor is not virtual
        /w34456 # Declaration of 'var' hides previous local declaration
        /w34457 # Declaration of 'var' hides function parameter
        /w34458 # Declaration of 'var' hides class member
        /w34459 # Declaration of 'var' hides global definition
        /w34946 # Reinterpret-cast between related types
        /wd4592 # Symbol will be dynamically initialized (implementation limitation)
        /permissive- # Stricter C++ standards conformance
        /MP
        /Zi
        /Zo
        /EHsc
        /Zc:throwingNew # Assumes new never returns null
        /Zc:inline # Omits inline functions from object-file output
        /DNOMINMAX
        /WX)

    if (CMAKE_VS_PLATFORM_TOOLSET MATCHES "LLVM-vs[0-9]+")
        list(APPEND SIRIT_CXX_FLAGS
             -Qunused-arguments
             -Wno-missing-braces)
    endif()
else()
    include(GNUInstallDirs)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wcast-qual -pedantic -pedantic-errors -Wfatal-errors -Wno-missing-braces -Wconversion -Wsign-conversion -Wshadow -Werror")
endif()

# SPIR-V headers
find_package(SPIRV-Tools QUIET)
if(SPIRV-Tools_NOTFOUND)
    MESSAGE(STATUS "SPIRV-Tools not found. Using bundled SPIRV-Tools")
    add_subdirectory(externals/SPIRV-Headers EXCLUDE_FROM_ALL)
endif(SPIRV-Tools_NOTFOUND)

# Sirit project files
add_subdirectory(src)

if(BUILD_TESTS)
    # Enable unit-testing.

    enable_testing(true)
    add_subdirectory(tests)
endif(BUILD_TESTS)

if(NOT MSVC)
    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_TARGETS
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${PROJECT_NAME}ConfigVersion.cmake"
        VERSION 
            ${PROJECT_VERSION}
        COMPATIBILITY 
            SameMajorVersion)
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
            ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

    install(EXPORT ${PROJECT_NAME}_TARGETS
            FILE 
                   ${PROJECT_NAME}Targets.cmake
            NAMESPACE 
                   ${PROJECT_NAME}::
            DESTINATION 
                   ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
    install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION
                  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
    install(DIRECTORY
                ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
            DESTINATION
                ${CMAKE_INSTALL_INCLUDEDIR})
endif(NOT MSVC)
