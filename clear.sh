#!/bin/sh

# Remove build directory.
echo '> rm -rv build'
rm -rv build

# Remove exec files.
echo '\n> rm -v *.out'
rm -v *.out

# Remove output directory.
echo '\n> rm -rv output'
rm -rv output
