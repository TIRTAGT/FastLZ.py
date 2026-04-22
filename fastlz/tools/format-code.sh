#!/usr/bin/env bash

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cwd=$(cd -- "${script_dir}/../.." && pwd)
clang-format-6.0 -i --style='{BasedOnStyle: "google", ColumnLimit: 120}' \
  $cwd/fastlz/vendor/*.h \
  $cwd/fastlz/vendor/*.c \
  $cwd/fastlz/tests_c/*.c \
  $cwd/fastlz/examples/*.c \
  $cwd/fastlz/_fastlzmodule.c
