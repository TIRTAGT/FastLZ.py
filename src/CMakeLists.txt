cmake_minimum_required(VERSION 4.3)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(pybind11 CONFIG REQUIRED)

file(GLOB_RECURSE SOURCE_FILES
	"./*.cpp"
	"./FastLZ/fastlz.c"
)

pybind11_add_module(
	fastlzpy
	${SOURCE_FILES}
)

install(TARGETS fastlzpy DESTINATION .)