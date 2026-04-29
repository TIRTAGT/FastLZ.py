#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstddef>

#if defined(_MSC_VER)

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#endif

namespace py = pybind11;

using std::pair;
using std::byte;
using std::unique_ptr;

#if DEBUG_MODE == 1
void buffer_info(py::buffer input);
#endif
bool is_memory_c_contiguous(const py::buffer_info& input_info);
pair<unique_ptr<byte[]>, size_t> copy_to_contiguous(const py::buffer_info& input_info);