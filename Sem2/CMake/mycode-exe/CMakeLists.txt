cmake_minimum_required(VERSION 3.10)

project(mycode-exe)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
elseif(MSVC)
	add_compile_options(/utf-8)
endif()

add_executable(${PROJECT_NAME}
	main.cpp
	function.cpp
)
