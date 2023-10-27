#!/bin/sh

SOURCE_TREE='.'
BUILD_TREE='build'


# Generate build system.
printf "> mkdir ${BUILD_TREE}\n"
mkdir $BUILD_TREE

printf "\n> cmake -S ${SOURCE_TREE} -B ${BUILD_TREE}\n"
cmake -S $SOURCE_TREE -B $BUILD_TREE

# Build.
printf "\n> cmake --build ${BUILD_TREE}\n"
cmake --build $BUILD_TREE
