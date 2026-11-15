#!/bin/bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --clean-first
ctest --test-dir build --output-on-failure
