cmake_minimum_required(VERSION 3.14)

project(mycode-tests) # Имя решения, содержащего "цели", targets
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
elseif(MSVC)
	add_compile_options(/utf-8)
endif()

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