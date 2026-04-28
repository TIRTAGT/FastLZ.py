#include "utility.h"
#include <limits>
#include <cstring>
#include <print>
#include <memory>

using std::overflow_error;
using std::range_error;
using std::invalid_argument;
using std::numeric_limits;
using std::println;
using std::make_unique;
using std::memcpy;
using std::move;

/**
 * @brief Checks whether a 1D pybind11 byte buffer is contiguous.
 *
 * @param input_info The buffer metadata to inspect.
 * @return true if the buffer is contiguous, empty, or scalar.
 * @throws std::range_error If buffer size/itemsize metadata is negative.
 * @throws std::invalid_argument If the buffer is not 1D or not a byte buffer.
 */
bool py_memory_is_contiguous(const py::buffer_info& input_info) {
	// No idea why .size and .itemsize are not unsigned...
	if (input_info.size < 0 || input_info.itemsize < 0) {
		throw range_error("Buffer size/entries must not be negative values");
	}

	if (input_info.size == 0 || input_info.itemsize == 0) {
		return true; // Empty buffers are considered contiguous
	}

	// We do not support other than 1 dimension
	if (input_info.ndim != 1) {
		throw invalid_argument("Only 1D buffers are supported");
	}

	if (input_info.format.empty() || (input_info.format.back() != 'B' && input_info.format.back() != 'b')) {
		throw invalid_argument("Only unsigned byte (0-255) buffers are supported");
	}

	// Safe cast because we reject negative values
	const size_t item_size = static_cast<size_t>(input_info.itemsize);

	// If fragmented, use python's strides
	ssize_t input_offset = item_size;
	if (input_info.strides.size() > 0) {
		input_offset = input_info.strides[0];
	}

	return input_offset == static_cast<ssize_t>(item_size);
}

#if DEBUG_MODE == 1
void matthew_debug(py::buffer input) {
	py::buffer_info input_info = input.request();

	println("Buffer info:");
	println("  ptr: {}", input_info.ptr);
	println("  itemsize: {}", input_info.itemsize);
	println("  size: {}", input_info.size);
	println("  format: {}", input_info.format);
	println("  ndim: {}", input_info.ndim);
	println("  Shape: {}", input_info.shape);
	println("  Strides: {}", input_info.strides);
	println("  Readonly: {}", input_info.readonly);
}
#endif

/**
 * @brief Copies a potentially strided 1D pybind11 byte buffer into a contiguous allocation.
 *
 * @param input_info The buffer metadata describing the source data.
 * @return A pair containing the contiguous buffer and its total size in bytes.
 * @throws std::range_error If buffer size/itemsize metadata is negative.
 * @throws std::invalid_argument If the buffer is not 1D or not a byte buffer.
 * @throws std::overflow_error If the total buffer size would exceed addressable memory.
 */
pair<unique_ptr<byte[]>, size_t> py_memory_copy_to_contiguous(const py::buffer_info& input_info) {
	// No idea why .size and .itemsize are not unsigned...
	if (input_info.size < 0 || input_info.itemsize < 0) {
		throw range_error("Buffer size/entries must not be negative values");
	}

	if (input_info.size == 0 || input_info.itemsize == 0) {
		return {nullptr, 0}; // No data to copy
	}

	// We do not support other than 1 dimension
	if (input_info.ndim != 1) {
		throw invalid_argument("Only 1D buffers are supported");
	}

	if (input_info.format.empty() || (input_info.format.back() != 'B' && input_info.format.back() != 'b')) {
		throw invalid_argument("Only unsigned byte (0-255) buffers are supported");
	}

	// Safe cast because we reject negative values
	const size_t item_size = static_cast<size_t>(input_info.itemsize);
	const size_t item_count = static_cast<size_t>(input_info.size);

	// Check if element_count * item_size would result in overflow
	if (item_count > numeric_limits<size_t>::max() / item_size) {
		throw overflow_error("Buffer size exceeds maximum representable size");
	}

	size_t total_size = item_count * item_size;

	byte* pInputBuffer = static_cast<byte*>(input_info.ptr);

	// If fragmented, use python's strides
	ssize_t input_offset = item_size;
	if (input_info.strides.size() > 0) {
		input_offset = input_info.strides[0];
	}

	unique_ptr<byte[]> cleanBuffer = make_unique<byte[]>(total_size);
	byte* pCleanBuffer = cleanBuffer.get();

	// If not fragmented and we can forward copy immediately
	if (input_offset == static_cast<ssize_t>(item_size)) {
		// Copy the whole buffer at once
		memcpy(pCleanBuffer, pInputBuffer, total_size);
	}
	else {
		// Copy all the elements
		for (size_t i = 0; i < item_count; ++i) {
			byte* pNextDestination = pCleanBuffer + (i * item_size);
			byte* pNextSource = pInputBuffer + (static_cast<ssize_t>(i) * input_offset);

			memcpy(pNextDestination, pNextSource, item_size);
		}
	}

	return {
		move(cleanBuffer),
		total_size
	};
}