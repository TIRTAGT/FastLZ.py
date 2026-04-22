from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            "fastlz._fastlz",
            sources=["fastlz/_fastlzmodule.c", "fastlz/vendor/fastlz.c"],
            include_dirs=["fastlz", "fastlz/vendor"],
        )
    ]
)
