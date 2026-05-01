#include <pybind11/pybind11.h>
#include "FastLZ/fastlz.h"
#include "utility.h"
#include <limits>
#include <memory>
#include <vector>

#if defined(_MSC_VER)

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#endif

namespace py = pybind11;

using std::numeric_limits;
using std::invalid_argument;
using std::overflow_error;
using std::pair;
using std::unique_ptr;
using std::vector;
using std::byte;

int compress_level(int level, py::buffer input, py::buffer output) {
	py::buffer_info input_info = input.request();
	py::buffer_info output_info = output.request();

	if (input_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	if (output_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Output size exceeds FastLZ maximum of 2^31-1");
	}

	if (!is_memory_c_contiguous(output_info)) {
		throw invalid_argument("Output buffer must be contiguous");
	}

	if (is_memory_c_contiguous(input_info)) {
		return fastlz_compress_level(
			level,
			input_info.ptr,
			(int) input_info.size,
			output_info.ptr
		);
	}

	pair<unique_ptr<byte[]>, size_t> input_copy = copy_to_contiguous(input_info);
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

	if (!is_memory_c_contiguous(output_info)) {
		throw invalid_argument("Output buffer must be contiguous");
	}

	if (is_memory_c_contiguous(input_info)) {
		return fastlz_decompress(
			input_info.ptr,
			(int) input_info.size,
			output_info.ptr,
			(int) output_info.size
		);
	}

	pair<unique_ptr<byte[]>, size_t> input_copy = copy_to_contiguous(input_info);
	return fastlz_decompress(
		input_copy.first.get(),
		(int) input_copy.second,
		output_info.ptr,
		(int) output_info.size
	);
}

int DEFAULT_MAX_ALLOC_SIZE = 1024 * 1024 * 8; // 8 MB

py::memoryview decompress_dynamic(py::buffer input, int max_output_size = DEFAULT_MAX_ALLOC_SIZE) {
	printf("TEST");

	if (max_output_size <= 0 || max_output_size > numeric_limits<int>::max()) {
		throw invalid_argument("Output size must be a positive integer less than or equal to 2^31 - 1");
	}

	#if DEBUG_MODE == 1
	printf("Starting decompression with max_output_size = %d bytes\n", max_output_size);
	#endif

	pair<unique_ptr<byte[]>, size_t> input_copy = copy_to_contiguous(input.request());

	#if DEBUG_MODE == 1
	printf("Input size after copying to contiguous buffer: %zu bytes\n", input_copy.second);
	#endif

	if (input_copy.second > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	if (input_copy.second == 0) {
		throw invalid_argument("Input must not be empty");
	}

	const void* input_ptr = input_copy.first.get();
	int input_size = static_cast<int>(input_copy.second);

	size_t current_alloc = static_cast<size_t>(input_size);
	size_t max_overflow_alloc = numeric_limits<size_t>::max() / 2;

	#if DEBUG_MODE == 1
	printf("Initial output buffer size set to input size: %zu bytes\n", current_alloc);
	#endif

	while (current_alloc <= max_output_size) {
		unique_ptr<byte[]> output(new byte[current_alloc]);
		int decompressed_size = fastlz_decompress(
			input_ptr,
			input_size,
			output.get(),
			current_alloc
		);

		if (decompressed_size > 0) {
			char* output_ptr = reinterpret_cast<char*>(output.get());
			py::bytes result(output_ptr, decompressed_size);
			return py::memoryview(result);
		}

		// Prevent potential overflow when doubling current_alloc
		if (current_alloc > max_overflow_alloc) {
			throw overflow_error("Output buffer overflow during decompression");
		}

		current_alloc = current_alloc * 2;

		#if DEBUG_MODE == 1
		printf("Increasing output buffer size to %zu bytes\n", current_alloc);
		#endif
	}

	throw overflow_error("Data is invalid or max_output_size is too small");
}

py::memoryview compress_level_dynamic(int level, py::buffer input, int max_output_size = DEFAULT_MAX_ALLOC_SIZE) {
	if (level < 1 || level > 2) {
		throw invalid_argument("Compression level must be 1 or 2");
	}

	if (max_output_size <= 0 || max_output_size > numeric_limits<int>::max()) {
		throw invalid_argument("max_output_size must be a positive integer less than or equal to 2^31 - 1");
	}

	py::buffer_info input_info = input.request();

	if (input_info.size > numeric_limits<int>::max()) {
		throw overflow_error("Input size exceeds FastLZ maximum of 2^31-1");
	}

	if (is_memory_c_contiguous(input_info)) {
		int current_alloc = static_cast<int>(input_info.size) + 400;
		while (current_alloc <= max_output_size) {
			vector<char> output(current_alloc);
			int compressed_size = fastlz_compress_level(
				level,
				input_info.ptr,
				(int) input_info.size,
				output.data()
			);

			if (compressed_size > 0) {
				py::bytes result(output.data(), compressed_size);
				return py::memoryview(result);
			}

			int new_limit = current_alloc * 2;
			if (new_limit > max_output_size) {
				new_limit = max_output_size;
			}
			else if (new_limit <= current_alloc) {
				throw overflow_error("Output buffer overflow during compression");
			}

			current_alloc = new_limit;
		}
	}
	else {
		pair<unique_ptr<byte[]>, size_t> input_copy = copy_to_contiguous(input_info);
		int current_alloc = static_cast<int>(input_copy.second) + 400;
		while (current_alloc <= max_output_size) {
			vector<char> output(current_alloc);
			int compressed_size = fastlz_compress_level(
				level,
				input_copy.first.get(),
				(int) input_copy.second,
				output.data()
			);

			if (compressed_size > 0) {
				py::bytes result(output.data(), compressed_size);
				return py::memoryview(result);
			}

			int new_limit = current_alloc * 2;
			if (new_limit > max_output_size) {
				new_limit = max_output_size;
			}
			else if (new_limit <= current_alloc) {
				throw overflow_error("Output buffer overflow during compression");
			}

			current_alloc = new_limit;
		}
	}

	throw overflow_error("max_output_size is too small for compression");
}

py::memoryview compress_dynamic(py::buffer input, int max_output_size = DEFAULT_MAX_ALLOC_SIZE) {
	return compress_level_dynamic(1, input, max_output_size);
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

	if (!is_memory_c_contiguous(output_info)) {
		throw invalid_argument("Output buffer must be contiguous");
	}

	if (is_memory_c_contiguous(input_info)) {
		return fastlz_compress(
			input_info.ptr,
			(int) input_info.size,
			output_info.ptr
		);
	}

	pair<unique_ptr<byte[]>, size_t> input_copy = copy_to_contiguous(input_info);
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
		"Compress a block of data in the input buffer",
		py::arg("level"),
		py::arg("input"),
		py::arg("output")
	);

	m.def(
		"fastlz_decompress",
		&decompress,
		"Decompress a block of compressed data",
		py::arg("input"),
		py::arg("output")
	);

	m.def(
		"fastlz_compress",
		&compress,
		"This is similar to fastlz_compress_level above, but with the level automatically chosen.",
		py::arg("input"),
		py::arg("output")
	);

	m.def(
		"fastlz_compress_level_dynamic",
		&compress_level_dynamic,
		"Compress a block with automatic output buffer sizing",
		py::arg("level"),
		py::arg("input"),
		py::arg("max_output_size") = DEFAULT_MAX_ALLOC_SIZE
	);

	m.def(
		"fastlz_compress_dynamic",
		&compress_dynamic,
		"Compress a block with automatic output buffer sizing and auto-selected level",
		py::arg("input"),
		py::arg("max_output_size") = DEFAULT_MAX_ALLOC_SIZE
	);

	m.def(
		"fastlz_decompress_dynamic",
		&decompress_dynamic,
		"Decompress a block with automatic output buffer sizing",
		py::arg("input"),
		py::arg("max_output_size") = DEFAULT_MAX_ALLOC_SIZE
	);

#if DEBUG_MODE == 1
	m.def(
		"buffer_info",
		&buffer_info,
		"This is a debug function"
	);

	m.def(
		"is_memory_c_contiguous",
		[](py::buffer input) -> bool {
			py::buffer_info info = input.request();
			return is_memory_c_contiguous(info);
		},
		"Check if a 1D byte buffer is contiguous"
	);

	m.def(
		"copy_to_contiguous",
		[](py::buffer input) -> py::memoryview {
			py::buffer_info info = input.request();
			pair<unique_ptr<byte[]>, size_t> result = copy_to_contiguous(info);
			if (result.first == nullptr) {
				return py::memoryview(py::bytes());
			}

			py::bytes contiguous(reinterpret_cast<const char*>(result.first.get()), result.second);
			return py::memoryview(contiguous);
		},
		"Copy a potentially strided 1D byte buffer into a contiguous Python bytes object"
	);
#endif

}