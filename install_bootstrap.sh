#!/bin/bash

if ! command -v g++ &> /dev/null
then
    echo "Error: g++ not found. Please install it first."
    exit 1
fi

if [ ! -d "build" ]; then
    mkdir build
fi

echo "Compiling install.cpp..."
g++ -o build/install install.cpp -std=c++17 -lstdc++fs

if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the install script..."
    ./build/install "$@"
else
    echo "Error: Compilation failed."
    exit 1
fi
