#!/bin/bash

set -e

mkdir -p build/Release
(cd build/Release; cmake -DCMAKE_BUILD_TYPE=Release ../..)
make -C build/Release
make -C build/Release test
clang-tidy-6.0 -header-filter=src/starkware -p=build/Release $(find src/starkware -name "*.cc")
cpplint --extensions=cc,h $(find src/starkware | grep -E '\.(cc|h)$')
