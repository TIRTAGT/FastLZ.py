import pytest

import fastlz


@pytest.mark.parametrize("level", [1, 2])
@pytest.mark.parametrize(
    "data",
    [
        b"",
        b"a",
        b"ab",
        b"abc",
        b"abcd",
        b"a" * 15,
        b"a" * 16,
        b"a" * 17,
        (b"abcdefg" * 4096),
        bytes(range(256)) * 256,
    ],
)
def test_roundtrip(level, data):
    compressed = fastlz.compress(data, level=level)
    out = fastlz.decompress(compressed, maxout=len(data))
    assert out == data


def test_readme_example_vectors_level1():
    assert fastlz.decompress(bytes([0x02, 0x41, 0x42, 0x43]), maxout=3) == b"ABC"
    assert fastlz.decompress(bytes([0x03, 0x41, 0x42, 0x43, 0x44, 0x20, 0x02]), maxout=7) == b"ABCDBCD"
    assert fastlz.decompress(bytes([0x00, 0x61, 0x40, 0x00]), maxout=5) == b"aaaaa"


def test_invalid_level_raises():
    with pytest.raises(ValueError):
        fastlz.compress(b"hello", level=3)


def test_corrupt_input_raises():
    raw = b"hello world" * 32
    compressed = bytearray(fastlz.compress(raw, level=2))
    compressed[0] = 0xE0
    with pytest.raises(ValueError):
        fastlz.decompress(compressed, maxout=len(raw))


def test_small_maxout_raises():
    raw = b"0123456789" * 32
    compressed = fastlz.compress(raw, level=1)
    with pytest.raises(ValueError):
        fastlz.decompress(compressed, maxout=len(raw) - 1)


def test_deterministic_output():
    data = b"repeat me" * 200
    c1 = fastlz.compress(data, level=1)
    c2 = fastlz.compress(data, level=1)
    assert c1 == c2


def test_max_compressed_size_contract():
    data = b"x" * 4096
    compressed = fastlz.compress(data)
    assert len(compressed) <= fastlz.max_compressed_size(len(data))
