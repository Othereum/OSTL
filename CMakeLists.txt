cmake_minimum_required(VERSION 3.8)
project("OSTL")
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC_FILES "OSTL-Test/*.cpp")
add_executable(main ${SRC_FILES})
target_include_directories(main PRIVATE "include")

enable_testing()
find_package(GTest CONFIG REQUIRED)
target_link_libraries(main PRIVATE GTest::gtest)
add_test(AllTestsInMain main)
