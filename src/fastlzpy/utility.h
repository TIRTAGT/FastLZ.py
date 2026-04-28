#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstddef>

namespace py = pybind11;

using std::pair;
using std::byte;
using std::unique_ptr;

#if DEBUG_MODE == 1
void matthew_debug(py::buffer input);
#endif
bool py_memory_is_contiguous(const py::buffer_info& input_info);
pair<unique_ptr<byte[]>, size_t> py_memory_copy_to_contiguous(const py::buffer_info& input_info);