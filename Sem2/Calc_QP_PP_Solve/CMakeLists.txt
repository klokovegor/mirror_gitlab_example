cmake_minimum_required(VERSION 3.14)
project(hydraulics_tasks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Tasks STATIC
    src/begining_tasks_hydraulics.cpp
    src/begining_tasks_hydraulics.h
)

add_executable(Tests
    test/test_begining_tasks_hydraulics.cpp
)

target_compile_definitions(Tests PRIVATE
    CALC_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

find_package(GTest REQUIRED)

target_link_libraries(Tests PRIVATE
    Tasks
    GTest::gtest 
    GTest::gtest_main
)

target_include_directories(Tasks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)