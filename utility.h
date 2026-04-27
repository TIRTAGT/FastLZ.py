#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstddef>

namespace py = pybind11;

using std::pair;
using std::byte;
using std::unique_ptr;

void matthew_debug(py::buffer input);
bool py_memory_is_contiguous(const py::buffer_info& input_info);
pair<unique_ptr<byte[]>, size_t> py_memory_copy_to_contiguous(py::buffer_info input_info);