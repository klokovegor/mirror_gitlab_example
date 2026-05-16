cmake_minimum_required(VERSION 3.14)
project(hydraulics_tasks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

add_library(Tasks STATIC
    src/hydraulic_chain.cpp
    src/hydraulic_chain.h
    src/pipe_oil.cpp
    src/pipe_oil.h
    src/local_resistance.cpp
    src/local_resistance.h
    src/pump.cpp
    src/pump.h
    src/numerical_methods.cpp
    src/numerical_methods.h
    src/logger_base.h
)

add_executable(Tests
    test/test_hydraulic_chain.cpp
    test/test_pipe_oil.cpp
    test/test_local_resistance.cpp
    test/test_pump.cpp
    test/test_numerical_methods.cpp
    test/test_logger_base.cpp
)

find_package(GTest REQUIRED)
find_package(fixed_solvers QUIET CONFIG)
if(NOT fixed_solvers_FOUND)
    FetchContent_Declare(
        fixed_solvers
        GIT_REPOSITORY https://github.com/victorsouth/fixed_solvers.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(fixed_solvers)
endif()

target_link_libraries(Tasks PRIVATE fixed_solvers::fixed_solvers)

target_link_libraries(Tests PRIVATE
    Tasks
    GTest::gtest_main
    fixed_solvers::fixed_solvers
)

target_include_directories(Tasks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)