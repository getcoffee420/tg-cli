#!/usr/bin/env bash
set -e

OS="$(uname)"

echo "Detected OS: $OS"

# --- Build TDLib ---
cd third_party/td

rm -rf build
mkdir build
cd build

if [[ "$OS" == "Darwin" ]]; then
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl/ \
          -DCMAKE_INSTALL_PREFIX:PATH=../tdlib \
          ..
else
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX:PATH=../tdlib \
          ..
fi

cmake --build . --target install

# Return to project root
cd ../../..

# --- Run cmake in project root ---
cmake .
