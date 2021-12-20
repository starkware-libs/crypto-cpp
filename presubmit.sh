#!/bin/bash

set -e

(cd src/starkware/crypto/ffi/js; npm install --no-package-lock)

mkdir -p build/Release
(cd build/Release; cmake -DCMAKE_BUILD_TYPE=Release ../..)
make -C build/Release
CTEST_OUTPUT_ON_FAILURE=1 make -C build/Release test
clang-tidy -header-filter=src/starkware -p=build/Release $(find src/starkware -name "*.cc" | grep -v node_modules)
cpplint --extensions=cc,h $(find src/starkware | grep -v node_modules | grep -E '\.(cc|h)$')
