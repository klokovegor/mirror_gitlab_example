cmake_minimum_required(VERSION 3.14)

project(mycode-tests) # Имя решения, содержащего "цели", targets
set(CMAKE_CXX_STANDARD 17)

## Библиотека с кодом заданий (статическая по умолчанию)
add_library(BooleanTasks STATIC # имя цели (аналог .vcxproj)
	src/BooleanTasks.cpp # единицы трансляции
)

## Сборка тестов
add_executable(BooleanTests # имя цели
	tests/TestBoolean.cpp # единицы трансляции
)

find_package(GTest REQUIRED)

target_link_libraries(BooleanTests PRIVATE
	BooleanTasks GTest::gtest GTest::gtest_main)

target_include_directories(BooleanTests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src)