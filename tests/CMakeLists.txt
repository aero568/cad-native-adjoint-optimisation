add_executable(test_find_span
    test_find_span.c
)

set_target_properties(test_find_span PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)

target_link_libraries(test_find_span
    PRIVATE
        cad_native_geometry
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(test_find_span
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
elseif(MSVC)
    target_compile_options(test_find_span
        PRIVATE
            /W4
    )
endif()

add_test(
    NAME find_span
    COMMAND test_find_span
)
