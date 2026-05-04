cmake_minimum_required(VERSION 3.16)
project(mini-dog-c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
include_directories(${CMAKE_SOURCE_DIR}/include)

# ===== Core library sources =====
set(LIB_SOURCES
    src/token.c
    src/lexer.c
    src/ast.c
    src/parser.c
    src/evaluator.c
)

# ===== Main executable =====
add_executable(mini-dog-c
    ${LIB_SOURCES}
    src/main.c
)

# ===== Test: lexer =====
add_executable(test_lexer
    ${LIB_SOURCES}
    tests/test_lexer.c
)

# ===== Test: parser =====
add_executable(test_parser
    ${LIB_SOURCES}
    tests/test_parser.c
)

# ===== Test: evaluator =====
add_executable(test_evaluator
    ${LIB_SOURCES}
    tests/test_evaluator.c
)

# ===== Run tests via ctest =====
enable_testing()

add_test(NAME lexer     COMMAND test_lexer)
add_test(NAME parser    COMMAND test_parser)
add_test(NAME evaluator COMMAND test_evaluator)

# Helper target to run all tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_lexer test_parser test_evaluator
    COMMENT "Running all tests"
)

# ===== "run" target: build and run example =====
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mini-dog-c ${CMAKE_CURRENT_SOURCE_DIR}/tests/example.mdc
    DEPENDS mini-dog-c
    COMMENT "Running example program"
)
