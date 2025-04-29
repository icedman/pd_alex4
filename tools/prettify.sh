#!/bin/sh

set -x
find ./src -iname '*.h' -o -iname '*.c' | xargs clang-format --verbose  -i -style=llvm
find ./alex -iname '*.h' -o -iname '*.c' | xargs clang-format --verbose  -i -style=llvm