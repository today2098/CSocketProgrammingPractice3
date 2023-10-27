#!/bin/sh

BUILD_TREE='build'
OUTPUT_DIR='output'


# Remove build tree.
printf "> rm -rv ${BUILD_TREE}\n"
rm -rv $BUILD_TREE

# Remove exe files.
printf "\n> rm -v *.out\n"
rm -v *.out

# Remove output directory.
printf "\n> rm -rv ${OUTPUT_DIR}\n"
rm -rv $OUTPUT_DIR
