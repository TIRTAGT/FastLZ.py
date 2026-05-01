#include "../src/FastLZ/fastlz.h"
#include "data.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <limits>

using std::vector;
using std::byte;
using std::numeric_limits;
using std::to_integer;

int main() {
	const size_t compressed_case_count = compressed_data.size();
	const size_t expected_case_count = decompressed_data.size();

	if (compressed_case_count != expected_case_count) {
		fprintf(
			stderr,
			"Dataset mismatch: %zu compressed cases vs %zu expected cases.\n",
			compressed_case_count,
			expected_case_count
		);
		return 1;
	}

	const size_t SAFE_INT_MAX = static_cast<size_t>(numeric_limits<int>::max());

	for (size_t case_index = 0; case_index < compressed_case_count; ++case_index) {
		const vector<byte>& compressed = compressed_data[case_index];
		const size_t compressed_size_t = compressed.size();
		if (compressed_size_t == 0 || compressed_size_t > SAFE_INT_MAX) {
			fprintf(
				stderr,
				"Compressed data for case %zu is too large for FastLZ (size: %zu bytes).\n",
				case_index,
				compressed_size_t
			);
			return 1;
		}
		const int compressed_size = static_cast<int>(compressed_size_t);

		const vector<byte>& expected = decompressed_data[case_index];
		const size_t expected_size_t = expected.size();
		if (expected_size_t == 0 || expected_size_t > SAFE_INT_MAX) {
			fprintf(
				stderr,
				"Expected data for case %zu is too large for FastLZ (size: %zu bytes).\n",
				case_index,
				expected_size_t
			);
			return 1;
		}
		const int expected_size = static_cast<int>(expected_size_t);

		vector<byte> output_data(expected.size());
		const int output_size = fastlz_decompress(
			compressed.data(),
			compressed_size,
			output_data.data(),
			expected_size
		);

		if (output_size <= 0) {
			fprintf(
				stderr,
				"FastLZ decompression failed for case %zu (returned: %d)\n",
				case_index,
				output_size
			);
			return 1;
		}

		if (output_size != expected_size) {
			fprintf(
				stderr,
				"Unexpected decompressed size in case %zu. Expected %zu bytes, got %d bytes.\n",
				case_index,
				expected.size(),
				output_size
			);
			return 1;
		}

		for (size_t i = 0; i < expected.size(); ++i) {
			if (output_data[i] != expected[i]) {
				fprintf(
					stderr,
					"Data mismatch in case %zu at index %zu: expected %u, got %u\n",
					case_index,
					i,
					to_integer<uint8_t>(expected[i]),
					to_integer<uint8_t>(output_data[i])
				);
				return 1;
			}
		}
	}

	printf(
		"Decompression successful and output matches expected data for %zu case(s).\n",
		compressed_case_count
	);
	return 0;
}