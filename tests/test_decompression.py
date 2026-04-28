import fastlzpy
import os

def test_compress_decompress_roundtrip(data: bytes, description: str):
	print(f"\n=== {description} ===")

	# Pre-allocate output buffer (worst case: input + 50 bytes arbitrary overhead)
	output_buf = bytearray(len(data) + 50)

	# Test compress
	compressed_size = fastlzpy.fastlz_compress(data, output_buf)
	print(f"Compressed size: {compressed_size}")
	assert compressed_size > 0, "Compression failed"

	# Test decompress with fixed-size output
	decompress_buf = bytearray(len(data))
	decompressed_size = fastlzpy.fastlz_decompress(
		memoryview(output_buf)[:compressed_size],
		decompress_buf
	)
	print(f"Decompressed size: {decompressed_size}")
	assert decompressed_size == len(data), f"Size mismatch: {decompressed_size} != {len(data)}"
	assert bytes(decompress_buf) == data, "Data mismatch after roundtrip"

	# Test decompress_dynamic
	dynamic_result = fastlzpy.fastlz_decompress_dynamic(
		memoryview(output_buf)[:compressed_size]
	)
	assert dynamic_result == data, "Dynamic decompression data mismatch"

	print("Roundtrip OK")

if __name__ == "__main__":
	test_array = bytes([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

	# Contiguous input
	test_compress_decompress_roundtrip(test_array, "Contiguous full array")

	# Contiguous slice
	contiguous_slice = memoryview(test_array)[2:8]
	test_compress_decompress_roundtrip(contiguous_slice, "Contiguous slice [2:8]")

	# Strided input (non-contiguous)
	strided = memoryview(test_array)[0:10:2]
	test_compress_decompress_roundtrip(strided, "Strided input [0:10:2]")

	# Negative stride (non-contiguous)
	neg_stride = memoryview(test_array)[9:2:-1]
	test_compress_decompress_roundtrip(neg_stride, "Negative stride input [9:2:-1]")

	# Larger data for better compression ratio
	larger_data = bytes(range(256)) * 100
	test_compress_decompress_roundtrip(larger_data, "Larger repeating data (25.6 KB)")

	# Uncompressible data (random bytes)
	random_data = os.urandom(128)
	test_compress_decompress_roundtrip(random_data, "Uncompressible random data (128 bytes)")

	print("\n=== All tests passed ===")
