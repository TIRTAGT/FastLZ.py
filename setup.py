from setuptools import setup, Extension
import pybind11

setup(
	name="fastlzpy",
	version="0.1.0",
	description="A Python wrapper for the FastLZ compression library",
	author="Matthew Tirtawidjaja",
	author_email="matthew@tirtagt.com",
	ext_modules=[
		Extension(
			name="fastlzpy",
			sources=["bridge.cpp", "utility.cpp", "FastLZ/fastlz.c"],
			include_dirs=[pybind11.get_include()],
			language="c++",
			extra_compile_args=["-std=c++26"]
		)
	],
)