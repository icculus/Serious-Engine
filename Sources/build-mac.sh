#!/bin/bash

NCPU=`sysctl -n hw.ncpu`
echo "Will build with 'make -j$NCPU' ... please edit this script if incorrect."

set -e
set -x

rm -rf cmake-build
mkdir $_
cd $_
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_ARCHITECTURES=i386 ..
make -j$NCPU

