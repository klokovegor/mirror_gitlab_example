cmake_minimum_required(VERSION 3.14)

project(mycode-tests) # Имя решения, содержащего "цели", targets
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
elseif(MSVC)
	add_compile_options(/utf-8)
endif()

## Библиотека с кодом заданий (статическая по умолчанию)
add_library(Tasks STATIC # имя цели (аналог .vcxproj)
	src/begining_tasks_hydraulics.cpp # единицы трансляции
)

## Сборка тестов
add_executable(Tests # имя цели
	test/test_begining_tasks_hydraulics.cpp # единицы трансляции
)

target_compile_definitions(Tests PRIVATE
	CALC_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

find_package(GTest REQUIRED)

target_link_libraries(Tests PRIVATE
	Tasks  GTest::gtest GTest::gtest_main)

target_include_directories(Tests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src)
