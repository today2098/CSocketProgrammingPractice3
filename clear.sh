#!/bin/sh

# Remove output files.
echo '> rm -r output'
rm -rf output

# Remove exec files.
echo '> rm *.out'
rm *.out

# Remove "build/" directory.
echo '> rm -r build'
rm -rf build
