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

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
cmake_minimum_required(VERSION 3.10)
project(simple-svg)

set(CMAKE_CXX_STANDARD 23)

add_executable(simple-svg main_1.0.0.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
