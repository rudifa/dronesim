# Find all test files
file(GLOB TEST_SOURCES "*.cpp")

# Create a list to store all test targets
set(ALL_TESTS "")

# Create a test executable for each test file
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE gtest gtest_main)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    list(APPEND ALL_TESTS ${TEST_NAME})
endforeach()

# Create a custom target to build all tests
add_custom_target(build_tests
    DEPENDS ${ALL_TESTS}
    COMMENT "Building all tests"
)

# Create a custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${ALL_TESTS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all tests"
)

include_directories(${CMAKE_SOURCE_DIR}/simple_svg)
include_directories(${GTEST_INCLUDE_DIRS})
