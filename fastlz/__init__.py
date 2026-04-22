from ._fastlz import (
    FASTLZ_VERSION_MAJOR,
    FASTLZ_VERSION_MINOR,
    FASTLZ_VERSION_REVISION,
    __fastlz_version__,
    compress,
    decompress,
    max_compressed_size,
)

__all__ = [
    "compress",
    "decompress",
    "max_compressed_size",
    "__fastlz_version__",
    "FASTLZ_VERSION_MAJOR",
    "FASTLZ_VERSION_MINOR",
    "FASTLZ_VERSION_REVISION",
]
