cmake_minimum_required(VERSION 3.31.2)
project(YourProjectName)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

# Configure testing
include(CTest)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Add custom targets for building and running tests
add_custom_target(tests
    DEPENDS build_tests
    COMMENT "Building all tests"
)

add_custom_target(check
    DEPENDS run_tests
    COMMENT "Running all tests"
)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Set the source directory for the simple-svg executable
set(SIMPLE_SVG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/simple_svg)

# Add the simple-svg executable
add_executable(simple_svg ${SIMPLE_SVG_SOURCE_DIR}/main_1.0.0.cpp)

# Set the output directory for the simple-svg executable
set_target_properties(simple_svg PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

get_target_property(SIMPLE_SVG_OUTPUT_DIR simple_svg RUNTIME_OUTPUT_DIRECTORY)
message("simple_svg will be built in: ${SIMPLE_SVG_OUTPUT_DIR}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add rules for msplot_demo
set(MSPLOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/msplot)

# Add the msplot_demo executable
add_executable(msplot_demo
    ${MSPLOT_SOURCE_DIR}/msplot_demo.cpp
)

# Set include directories for msplot_demo
target_include_directories(msplot_demo PRIVATE
    ${MSPLOT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/simple_svg
)

# Set the output directory for the msplot_demo executable
set_target_properties(msplot_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

get_target_property(MSPLOT_DEMO_OUTPUT_DIR msplot_demo RUNTIME_OUTPUT_DIRECTORY)
message("msplot_demo will be built in: ${MSPLOT_DEMO_OUTPUT_DIR}")

# Add a custom target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests"
)

# Add rules for dronesim
set(DRONESIM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/dronesim)

# Instead of creating a new target, we'll modify the existing one
if(TARGET dronesim)
    # Set the output directory for the dronesim executable
    set_target_properties(dronesim PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    )

    get_target_property(DRONESIM_OUTPUT_DIR dronesim RUNTIME_OUTPUT_DIRECTORY)
    message("dronesim will be built in: ${DRONESIM_OUTPUT_DIR}")
else()
    message(WARNING "dronesim target not found. Make sure it's defined in src/dronesim/CMakeLists.txt")
endif()
