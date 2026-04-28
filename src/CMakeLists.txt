cmake_minimum_required(VERSION 4.3)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

# Find pybind11 (provided by the 'requires' in pyproject.toml)
find_package(pybind11 CONFIG REQUIRED)

# Define the extension module
# The name should match the module name in your C++ code
pybind11_add_module(fastlzpy fastlzpy/bridge.cpp)

# Install the module into the package directory
install(TARGETS fastlzpy DESTINATION fastlzpy)