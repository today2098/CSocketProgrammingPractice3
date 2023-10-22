#!/bin/sh

# Generate.
echo '> mkdir build'
mkdir build

echo -e '\n> cmake -S . -B build'
cmake -S . -B build

# Build.
echo -e '\n> cmake --build build'
cmake --build build
