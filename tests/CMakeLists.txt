cmake_minimum_required(VERSION 3.20)
project(fastlz_cpp_tests LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
	fastlz_cpp_tests
	main.cpp
	../src/FastLZ/fastlz.c
)

target_include_directories(
	fastlz_cpp_tests
	PRIVATE
	../src/FastLZ
)
