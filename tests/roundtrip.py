import fastlzpy
import unittest
import os

class RoundtripTestCase(unittest.TestCase):
	input_array = memoryview(bytes([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]))

	def __byte_compare(self, expected: bytes, actual: bytes) -> bool:
		expected_length = len(expected)
		actual_length = len(actual)

		if expected_length != actual_length:
			print(f"__byte_compare: actual size ({actual_length}) didn't match expected size ({expected_length})")
			return False
		
		for i in range(actual_length):
			if expected[i] != actual[i]:
				print(f"__byte_compare: Expecting data[{i}] to be {expected[i]}, but got {actual[i]}")
				return False

		return True
	
	def __test_compress_decompress_roundtrip(self, data: bytes):
		input_size = len(data)

		# Pre-allocate output buffer (worst case: input + 50 bytes arbitrary overhead)
		compressed_bytearray = bytearray(len(data) + 50)

		# Test compress
		compressed_size = fastlzpy.fastlz_compress(data, len(data), compressed_bytearray)
		if compressed_size <= 0:
			raise RuntimeError("Compression failed")
		compressed = memoryview(compressed_bytearray)

		# Test decompress with fixed-size output
		decompress_buf = bytearray(input_size)
		decompressed_size = fastlzpy.fastlz_decompress(
			compressed[:compressed_size],
			len(compressed[:compressed_size]),
			decompress_buf,
			len(decompress_buf)
		)

		if decompressed_size != input_size:
			raise RuntimeError(f"Size mismatch: {decompressed_size} != ${input_size}")
		
		if not self.__byte_compare(data, decompress_buf):
			raise RuntimeError("Fixed-size decompression data mismatch")

		# Test decompress_dynamic
		dynamic_result = fastlzpy.fastlz_decompress_dynamic(compressed[:compressed_size])
		if not self.__byte_compare(data, dynamic_result):
			raise RuntimeError("Dynamic decompression data mismatch")

	def test_contiguous_full_array(self):
		self.__test_compress_decompress_roundtrip(RoundtripTestCase.input_array)

	def test_contiguous_slice(self):
		contiguous_slice = RoundtripTestCase.input_array[2:8]
		self.__test_compress_decompress_roundtrip(contiguous_slice)

	def test_positive_stride(self):
		# Strided input (non-contiguous)
		strided = RoundtripTestCase.input_array[0:10:2]
		self.__test_compress_decompress_roundtrip(strided)

	def test_negative_stride(self):
		# Negative stride (non-contiguous)
		neg_stride = RoundtripTestCase.input_array[9:2:-1]
		self.__test_compress_decompress_roundtrip(neg_stride)

	def test_repeating_data(self):
		# Larger data for better compression ratio
		larger_data = bytes(range(256)) * 100
		self.__test_compress_decompress_roundtrip(larger_data)

	def test_random_data(self):
		random_data = os.urandom(128)
		self.__test_compress_decompress_roundtrip(random_data)

if __name__ == "__main__":
	unittest.main()
