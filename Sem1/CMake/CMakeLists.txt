cmake_minimum_required(VERSION 3.14)

project(mycode-tests) # Имя решения, содержащего "цели", targets
set(CMAKE_CXX_STANDARD 17)

## Библиотека с кодом заданий (статическая по умолчанию)
add_library(Tasks STATIC # имя цели (аналог .vcxproj)
	src/BooleanTasks.cpp src/CaseTasks.cpp
	src/ForTasks.cpp src/IntegerTasks.cpp
	src/STL1IterTasks.cpp src/STL2SeqTasks.cpp
	src/STL3AlgTasks.cpp src/WhileTasks.cpp
	src/IfTasks.cpp # единицы трансляции
)

## Сборка тестов
add_executable(Tests # имя цели
	Tests/TestBoolean.cpp Tests/TestCase.cpp
	Tests/TestFor.cpp Tests/TestInteger.cpp
	Tests/TestSTL1Iter.cpp Tests/TestSTL2Seq.cpp
	Tests/TestSTL3Alg.cpp Tests/TestWhile.cpp
	Tests/TestIf.cpp # единицы трансляции
)

find_package(GTest REQUIRED)

target_link_libraries(Tests PRIVATE
	Tasks  GTest::gtest GTest::gtest_main)

target_include_directories(Tests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src)