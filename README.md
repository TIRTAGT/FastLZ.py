# FastLZ.py

Python wrapper for the [FastLZ](https://github.com/ariya/fastlz) C library, based on latest `b1342dabcf5257ab303743c9332fe75e9147a011` commit (`master` at the time of creation of this repo).

## Building

### Build Dependencies
- C++26 compatible compiler (Tested on GCC 15+ or Clang 22+)
- [CMake](https://cmake.org/) v4.3+

### Python Build Dependencies
- [build](https://pypi.org/project/build/) v1.4+
- [pybind11-stubgen](https://pypi.org/project/pybind11-stubgen/)
- [pybind11](https://pypi.org/project/pybind11/) v3.0+
- [scikit-build-core](https://pypi.org/project/scikit-build-core/) v0.12+

### Build Instructions

1. Clone the repository and navigate to the project directory:
	```bash
	git clone https://github.com/TIRTAGT/FastLZ.py.git
	cd FastLZ.py
	```

2. (Optional) Create and activate a virtual environment:
	```bash
	python -m venv venv
	source venv/bin/activate
	```

	> For Windows users, activate with `venv\Scripts\activate`.

3. Install python build dependencies listed above:
	```bash
	pip install build pybind11-stubgen pybind11 scikit-build-core
	```

4. Build the module:
	```
	python -m build
	```

This will create a wheel file (`*.whl`) in the `dist/` directory.

To install it, run:

```bash
pip install dist/fastlzpy-*.whl
```

### Debug Mode

The `DEBUG_MODE` flag controls debug functionality. By default, `DEBUG_MODE=1` enables the `matthew_debug` function. To disable debug features, compile with `-DDEBUG_MODE=0`:

```bash
DEBUG_MODE=0 python setup.py build_ext --inplace
```

Or add `#define DEBUG_MODE 0` to your source before including the headers.

## Usage

The module is named `fastlzpy`.

### Compression

```python
import fastlzpy

data = b"Hello, World! Hello, World! Hello, World!"

# Pre-allocate output buffer (worst case: input + 400 bytes)
output_buf = bytearray(len(data) + 400)

# Auto-select compression level
compressed_size = fastlzpy.fastlz_compress(data, output_buf)
compressed = output_buf[:compressed_size]

# Or specify level explicitly (1 or 2)
compressed_size = fastlzpy.fastlz_compress_level(2, data, output_buf)
```

### Decompression

```python
import fastlzpy

# Fixed-size output (must know decompressed size in advance)
decompress_buf = bytearray(len(original_data))
decompressed_size = fastlzpy.fastlz_decompress(compressed, decompress_buf)
assert decompressed_size == len(original_data)

# Dynamic output sizing (auto-allocates, returns bytes)
decompressed = fastlzpy.fastlz_decompress_dynamic(compressed)
```

### Buffer Protocol Support

All functions accept any Python object implementing the buffer protocol (`bytes`, `bytearray`, `memoryview`, NumPy arrays, etc.).

**Strided / non-contiguous inputs are handled automatically** — the module copies them into contiguous memory before passing to FastLZ. The output buffer, however, **must be contiguous**.

```python
import fastlzpy

arr = bytearray(range(256))

# Strided view (every 2nd byte) — works transparently
strided = memoryview(arr)[0:256:2]

output_buf = bytearray(300)
compressed_size = fastlzpy.fastlz_compress(strided, output_buf)
```

### Utility Functions

```python
import fastlzpy

# Check if a 1D byte buffer is contiguous
fastlzpy.py_memory_is_contiguous(memoryview(b"hello"))  # True

# Copy a potentially strided buffer into a contiguous bytes object
contiguous = fastlzpy.py_memory_copy_to_contiguous(memoryview(b"hello")[::-1])

# Debug: print buffer metadata
fastlzpy.matthew_debug(memoryview(b"hello"))
```

## Limitations

- FastLZ operates on 32-bit `int` sizes. Input and output buffers are limited to **2,147,483,647 bytes** (~2 GB). Larger buffers will raise `OverflowError`.
- Only **1D byte buffers** (`format` ending in `B` or `b`) are supported. Multi-dimensional arrays or non-byte types will raise `ValueError`.
- Output buffers for `fastlz_compress`, `fastlz_compress_level`, and `fastlz_decompress` must be pre-allocated and contiguous.

## API Reference

| Function | Signature | Description |
|----------|-----------|-------------|
| `fastlz_compress` | `(input: Buffer, output: Buffer) -> int` | Compress with auto-selected level. Returns compressed size. |
| `fastlz_compress_level` | `(level: int, input: Buffer, output: Buffer) -> int` | Compress with level 1 or 2. Returns compressed size. |
| `fastlz_decompress` | `(input: Buffer, output: Buffer) -> int` | Decompress into pre-allocated buffer. Returns decompressed size, or 0 on error. |
| `fastlz_decompress_dynamic` | `(input: Buffer, max_output_size: int = 8_388_608) -> bytes` | Decompress with automatic buffer sizing. Returns `bytes`. |
| `py_memory_is_contiguous` | `(input: Buffer) -> bool` | Returns `True` if the buffer is 1D, contiguous, and byte-formatted. |
| `py_memory_copy_to_contiguous` | `(input: Buffer) -> bytes` | Copies a strided 1D byte buffer into a contiguous `bytes` object. |
| `matthew_debug` | `(input: Buffer) -> None` | Prints buffer metadata (ptr, shape, strides, format, etc.). Only available when `DEBUG_MODE=1`. |
