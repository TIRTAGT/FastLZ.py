cmake_minimum_required(VERSION 4.3)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(pybind11 CONFIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

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

set(STUBS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/stubs")
set(STUBS_OUTPUT_FILE "${STUBS_OUTPUT_DIR}/fastlzpy.pyi")

add_custom_command(
	TARGET fastlzpy
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${STUBS_OUTPUT_DIR}"
	COMMAND ${CMAKE_COMMAND} -E env
		PYTHONPATH=$<TARGET_FILE_DIR:fastlzpy>
		${Python3_EXECUTABLE} -m pybind11_stubgen fastlzpy --output-dir "${STUBS_OUTPUT_DIR}"
	BYPRODUCTS "${STUBS_OUTPUT_FILE}"
	COMMENT "Generating Python stubs for fastlzpy"
)

install(TARGETS fastlzpy DESTINATION .)
install(FILES "${STUBS_OUTPUT_FILE}" DESTINATION .)