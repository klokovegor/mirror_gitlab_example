cmake_minimum_required(VERSION 3.14)
project(klokov.egor7 NONE)

# Кодировка исходников и исполняемых строк — UTF-8 (русские символы)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
elseif(MSVC)
	add_compile_options(/utf-8)
endif()

add_subdirectory(Sem2/Calc)
