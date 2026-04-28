import fastlzpy

if __name__ == "__main__":
	test_array = memoryview(bytes([1, 2, 3, 4, 5]))

	print("Trying full array:")
	fastlzpy.buffer_info(test_array)
	print("  is_contiguous:", fastlzpy.is_memory_c_contiguous(test_array))
	
	result = list(fastlzpy.copy_to_contiguous(test_array))
	print("  copy:", result)
	print("  copy contiguous:", fastlzpy.is_memory_c_contiguous(bytes(result)))
	print()

	print("Trying slicing array (2, 3, 4):")
	fastlzpy.buffer_info(test_array[1:4])
	print("  is_contiguous:", fastlzpy.is_memory_c_contiguous(test_array[1:4]))

	result = list(fastlzpy.copy_to_contiguous(test_array[1:4]))
	print("  copy:", result)
	print("  copy contiguous:", fastlzpy.is_memory_c_contiguous(bytes(result)))
	print()

	print("Trying slicing array with custom step (1, 3, 5):")
	fastlzpy.buffer_info(test_array[0:5:2])
	print("  is_contiguous:", fastlzpy.is_memory_c_contiguous(test_array[0:5:2]))
	
	result = list(fastlzpy.copy_to_contiguous(test_array[0:5:2]))
	print("  copy:", result)
	print("  copy contiguous:", fastlzpy.is_memory_c_contiguous(bytes(result)))
	print()

	print("Trying negative slicing array (5, 4, 3):")
	fastlzpy.buffer_info(test_array[4:1:-1])
	print("  is_contiguous:", fastlzpy.is_memory_c_contiguous(test_array[4:1:-1]))

	result = list(fastlzpy.copy_to_contiguous(test_array[4:1:-1]))
	print("  copy:", result)
	print("  copy contiguous:", fastlzpy.is_memory_c_contiguous(bytes(result)))
