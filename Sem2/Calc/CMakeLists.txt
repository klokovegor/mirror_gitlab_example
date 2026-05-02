cmake_minimum_required(VERSION 3.14)
project(hydraulics_tasks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Tasks STATIC
    src/hydraulic_chain.cpp
    src/hydraulic_chain.h
    src/pipe_oil.cpp
    src/pipe_oil.h
    src/local_resistance.cpp
    src/local_resistance.h
    src/pump.cpp
    src/pump.h
)

add_executable(Tests
    test/test_hydraulic_chain.cpp
    test/test_pipe_oil.cpp
    test/test_local_resistance.cpp
    test/test_pump.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(Tests PRIVATE
    Tasks
    GTest::gtest_main
)

target_include_directories(Tasks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)