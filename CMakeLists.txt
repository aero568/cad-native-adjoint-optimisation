cmake_minimum_required(VERSION 3.20)

project(cad_native_adjoint_optimisation
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cad_native_geometry STATIC
    src/nurbs/cad_native.c
    src/nurbs/bspline.c
    src/nurbs/helpers.c
)

set_target_properties(cad_native_geometry PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(cad_native_geometry
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
elseif(MSVC)
    target_compile_options(cad_native_geometry
        PRIVATE
            /W4
    )
endif()

target_include_directories(cad_native_geometry
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/private
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
