# FastLZ.py

Python wrapper for the vendored [FastLZ](https://github.com/ariya/fastlz) C library.

## Layout

- `fastlz/vendor/`: vendored upstream FastLZ C sources (`fastlz.c`, `fastlz.h`)
- `fastlz/_fastlzmodule.c`: CPython extension binding layer
- `fastlz/examples/`, `fastlz/tests_c/`, `fastlz/tools/`: relocated C-only assets
- `tests/`: Python wrapper tests

## Installation

```bash
python -m pip install .
```

For development and tests:

```bash
python -m pip install -e .[test]
python -m pytest
```

## Python API

```python
import fastlz

payload = b"hello" * 100
compressed = fastlz.compress(payload, level=1)  # level: 1 or 2
restored = fastlz.decompress(compressed, maxout=len(payload))
assert restored == payload
```

`decompress()` requires `maxout` to preserve C API semantics (`fastlz_decompress(input, length, output, maxout)`).
Invalid/corrupt data or too-small output limits raise `ValueError`.

Helper:

```python
fastlz.max_compressed_size(input_length)
```

## C compatibility

The wrapper calls the real vendored C implementation directly.
Compression/decompression logic is not reimplemented in Python.

## Running relocated C tests/examples

```bash
# Round-trip corpus tests (requires compression-corpus checkout at repo root)
cd fastlz/tests_c && make roundtrip

# Build examples
cd ../examples && make
```

## License

MIT. Upstream FastLZ license and attribution are preserved in vendored C sources.
