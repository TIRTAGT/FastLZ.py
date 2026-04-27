#include <pybind11/pybind11.h>
#include "FastLZ/fastlz.h"
#include "utility.h"
#include <limits>
#include <memory>
#include <vector>

namespace py = pybind11;

using std::numeric_limits;
using std::invalid_argument;
using std::overflow_error;
using std::pair;
using std::unique_ptr;
using std::vector;

int compress_level(int level, py::buffer input, py::buffer output) {
	py::buffer_info input_info = input.request();
	py::buffer_info output_info = output.request();

	if (input_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	if (output_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Output size exceeds FastLZ maximum of 2^31-1");
	}

	if (!py_memory_is_contiguous(output_info)) {
		throw invalid_argument("Output buffer must be contiguous");
	}

	if (py_memory_is_contiguous(input_info)) {
		return fastlz_compress_level(
			level,
			input_info.ptr,
			(int) input_info.size,
			output_info.ptr
		);
	}

	pair<unique_ptr<std::byte[]>, size_t> input_copy = py_memory_copy_to_contiguous(input_info);
	return fastlz_compress_level(
		level,
		input_copy.first.get(),
		(int) input_copy.second,
		output_info.ptr
	);
}

int decompress(py::buffer input, py::buffer output) {
	py::buffer_info input_info = input.request();
	py::buffer_info output_info = output.request();

	if (input_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	if (output_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Output size exceeds FastLZ maximum of 2^31-1");
	}

	if (!py_memory_is_contiguous(output_info)) {
		throw invalid_argument("Output buffer must be contiguous");
	}

	if (py_memory_is_contiguous(input_info)) {
		return fastlz_decompress(
			input_info.ptr,
			(int) input_info.size,
			output_info.ptr,
			(int) output_info.size
		);
	}

	pair<unique_ptr<std::byte[]>, size_t> input_copy = py_memory_copy_to_contiguous(input_info);
	return fastlz_decompress(
		input_copy.first.get(),
		(int) input_copy.second,
		output_info.ptr,
		(int) output_info.size
	);
}

int DEFAULT_MAX_ALLOC_SIZE = 1024 * 1024 * 8; // 8 MB

py::bytes decompress_dynamic(py::buffer input, int max_output_size = DEFAULT_MAX_ALLOC_SIZE) {
	if (max_output_size <= 0 || max_output_size > numeric_limits<int>::max()) {
		throw invalid_argument("Output size must be a positive integer less than or equal to 2^31 - 1");
	}

	py::buffer_info input_info = input.request();

	if (input_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	pair<unique_ptr<std::byte[]>, size_t> input_copy = py_memory_copy_to_contiguous(input_info);

	const void* input_ptr = input_copy.first.get();
	ssize_t input_size = static_cast<ssize_t>(input_copy.second);

	int current_alloc = input_size * 4;
	while (current_alloc <= max_output_size) {
		vector<char> output(current_alloc);
		int decompressed_size = fastlz_decompress(
			input_ptr,
			(int) input_size,
			output.data(),
			current_alloc
		);

		if (decompressed_size > 0) {
			return py::bytes(output.data(), decompressed_size);
		}

		int new_limit = current_alloc * 2;
		if (new_limit > max_output_size) {
			new_limit = max_output_size;
		}
		else if (new_limit <= current_alloc) {
			throw overflow_error("Data is invalid or too large to decompress");
		}

		current_alloc = new_limit;
	}

	throw overflow_error("Data is invalid or max_output_size is too small");
}

int compress(py::buffer input, py::buffer output) {
	py::buffer_info input_info = input.request();
	py::buffer_info output_info = output.request();

	if (input_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	if (output_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Output size exceeds FastLZ maximum of 2^31-1");
	}

	if (!py_memory_is_contiguous(output_info)) {
		throw invalid_argument("Output buffer must be contiguous");
	}

	if (py_memory_is_contiguous(input_info)) {
		return fastlz_compress(
			input_info.ptr,
			(int) input_info.size,
			output_info.ptr
		);
	}

	pair<unique_ptr<std::byte[]>, size_t> input_copy = py_memory_copy_to_contiguous(input_info);
	return fastlz_compress(
		input_copy.first.get(),
		(int) input_copy.second,
		output_info.ptr
	);
}

PYBIND11_MODULE(fastlzpy, m) {
	m.def(
		"fastlz_compress_level",
		&compress_level,
		"Compress a block of data in the input buffer");

	m.def(
		"fastlz_decompress",
		&decompress,
		"Decompress a block of compressed data"
	);

	m.def(
		"fastlz_compress",
		&compress,
		"This is similar to fastlz_compress_level above, but with the level automatically chosen."
	);

	m.def(
		"fastlz_decompress_dynamic",
		&decompress_dynamic,
		"Decompress a block with automatic output buffer sizing",
		py::arg("input"),
		py::arg("max_output_size") = DEFAULT_MAX_ALLOC_SIZE
	);

	m.def(
		"matthew_debug",
		&matthew_debug,
		"This is a debug function"
	);

	m.def(
		"py_memory_is_contiguous",
		[](py::buffer input) -> bool {
			py::buffer_info info = input.request();
			return py_memory_is_contiguous(info);
		},
		"Check if a 1D byte buffer is contiguous"
	);

	m.def(
		"py_memory_copy_to_contiguous",
		[](py::buffer input) -> py::bytes {
			py::buffer_info info = input.request();
			pair<unique_ptr<std::byte[]>, size_t> result = py_memory_copy_to_contiguous(info);
			if (result.first == nullptr) {
				return py::bytes();
			}
			return py::bytes(reinterpret_cast<const char*>(result.first.get()), result.second);
		},
		"Copy a potentially strided 1D byte buffer into a contiguous Python bytes object"
	);
}