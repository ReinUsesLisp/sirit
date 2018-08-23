add_library(sirit
    sirit.cpp
)
target_include_directories(sirit
                           PUBLIC ../include ${SPIRV-Headers_SOURCE_DIR}/include}
                           PRIVATE .)
