#!/bin/bash

NCPU=`cat /proc/cpuinfo |grep vendor_id |wc -l`
let NCPU=$NCPU+2
echo "Will build with 'make -j$NCPU' ... please edit this script if incorrect."

set -e
set -x

rm -rf cmake-build
mkdir $_
cd $_
#cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 ..
#ninja

# This is the eventual path for amd64.
#cmake -DCMAKE_BUILD_TYPE=Debug ..

# Right now we force x86, though...
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DUSE_I386_NASM_ASM=TRUE .. $1

make -j$NCPU

