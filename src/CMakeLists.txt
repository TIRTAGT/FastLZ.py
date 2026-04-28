cmake_minimum_required(VERSION 4.3)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(pybind11 CONFIG REQUIRED)

option(DEBUG_MODE "Enable debug-only bindings" OFF)

file(GLOB_RECURSE SOURCE_FILES
	"./*.cpp"
	"./FastLZ/fastlz.c"
)

pybind11_add_module(
	fastlzpy
	${SOURCE_FILES}
)

target_compile_definitions(fastlzpy PRIVATE DEBUG_MODE=$<IF:$<BOOL:${DEBUG_MODE}>,1,0>)

install(TARGETS fastlzpy DESTINATION .)