set(target metric_test)

add_executable(${target}
    tests/code_lines_count.cpp
    tests/cyclomatic_complexity.cpp
    tests/parameters_count.cpp
)

target_link_libraries(${target}
    PRIVATE
        GTest::GTest
        GTest::Main
        metric
        function
        file
)


set(TEST_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/files")

target_compile_definitions(${target} 
    PRIVATE TEST_FILES_DIR="${TEST_FILES_DIR}"
)

add_test(NAME ${target} COMMAND ${target})
