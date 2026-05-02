import fastlzpy
import unittest

class MemoryCopyTestCase(unittest.TestCase):
	test_array = memoryview(bytes([1, 2, 3, 4, 5]))

	def test_full_array(self):
		fastlzpy.buffer_info(MemoryCopyTestCase.test_array)
		self.assertTrue(fastlzpy.is_memory_c_contiguous(MemoryCopyTestCase.test_array))
	
		result = fastlzpy.copy_to_contiguous(MemoryCopyTestCase.test_array)
		print("  copy:", list(result))
		self.assertTrue(fastlzpy.is_memory_c_contiguous(result))

	def test_normal_slicing(self):
		input_data = MemoryCopyTestCase.test_array[1:4]
		fastlzpy.buffer_info(input_data)
		self.assertTrue(fastlzpy.is_memory_c_contiguous(input_data))

		result = fastlzpy.copy_to_contiguous(input_data)
		print("  copy:", list(result))
		self.assertTrue(fastlzpy.is_memory_c_contiguous(result))

	def test_forward_step_slicing(self):
		"Tests slicing array with custom step (1, 3, 5)"

		input_data = MemoryCopyTestCase.test_array[0:5:2]
		fastlzpy.buffer_info(input_data)
		self.assertFalse(fastlzpy.is_memory_c_contiguous(input_data))
		
		result = fastlzpy.copy_to_contiguous(input_data)
		print("  copy:", list(result))
		self.assertTrue(fastlzpy.is_memory_c_contiguous(result))

	def test_negative_step_slicing(self):
		"Tests slicing array with negative step (5, 4, 3)"

		input_data = MemoryCopyTestCase.test_array[4:1:-1]

		fastlzpy.buffer_info(input_data)
		self.assertFalse(fastlzpy.is_memory_c_contiguous(input_data))

		result = fastlzpy.copy_to_contiguous(input_data)
		print("  copy:", list(result))
		self.assertTrue(fastlzpy.is_memory_c_contiguous(result))

if __name__ == "__main__":
	unittest.main()
