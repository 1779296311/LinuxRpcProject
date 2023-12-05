#!/usr/bin/env bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")"

THREAD_NUM=$(nproc)

# https://github.com/Kitware/CMake/archive/refs/tags/v3.26.4.tar.gz
# Install cmake.
VERSION="0.8.0"
PKG_NAME="yaml-cpp-${VERSION}.tar.gz"

tar xzf "${PKG_NAME}"
pushd yaml-cpp-${VERSION}
make -j${THREAD_NUM}
make install
popd

rm -rf PKG_NAME yaml-cpp-${VERSION}