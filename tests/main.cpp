#include "../FastLZ/fastlz.h"
#include "data.cpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

using std::vector;

int main() {
	const size_t compressed_case_count = sizeof(test_compressed_data) / sizeof(test_compressed_data[0]);
	const size_t expected_case_count =
		sizeof(expected_test_decompressed_data) / sizeof(expected_test_decompressed_data[0]);

	if (compressed_case_count != expected_case_count) {
		fprintf(
			stderr,
			"Dataset mismatch: %zu compressed cases vs %zu expected cases.\n",
			compressed_case_count,
			expected_case_count
		);
		return 1;
	}

	for (size_t case_index = 0; case_index < compressed_case_count; ++case_index) {
		const vector<uint8_t>& compressed = test_compressed_data[case_index];
		const vector<uint8_t>& expected = expected_test_decompressed_data[case_index];

		vector<uint8_t> decompressed_data(expected.size());
		const int decompressed_size = fastlz_decompress(
			compressed.data(),
			static_cast<int>(compressed.size()),
			decompressed_data.data(),
			static_cast<int>(expected.size())
		);

		if (decompressed_size <= 0) {
			fprintf(
				stderr,
				"FastLZ decompression failed for case %zu (returned: %d)\n",
				case_index,
				decompressed_size
			);
			return 1;
		}

		if (decompressed_size != static_cast<int>(expected.size())) {
			fprintf(
				stderr,
				"Unexpected decompressed size in case %zu. Expected %zu bytes, got %d bytes.\n",
				case_index,
				expected.size(),
				decompressed_size
			);
			return 1;
		}

		for (size_t i = 0; i < expected.size(); ++i) {
			if (decompressed_data[i] != expected[i]) {
				fprintf(
					stderr,
					"Data mismatch in case %zu at index %zu: expected %u, got %u\n",
					case_index,
					i,
					static_cast<unsigned>(expected[i]),
					static_cast<unsigned>(decompressed_data[i])
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