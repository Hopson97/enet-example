#!/bin/bash

target_release() {
    cd release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
    echo "Built target in bin/release/"
    cd ../..
}

target_debug() {
    cd debug 
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make
    echo "Built target in bin/debug/"
    cd ../..
}

# Create folder for distribution
if [ "$1" = "release" ]
then
    if [ -d "$recty" ]
    then
        rm -rf -d recty
    fi

    mkdir -p recty
fi

# Creates the folder for the binaries
mkdir -p bin
cd bin 
mkdir -p release
mkdir -p debug

# Builds target
if [ "$1" = "release" ]
then
    target_release
    cp bin/release/recty recty/recty
    cp bin/release/recty_server recty/recty_server
else
    target_debug
fi

cp -R res recty/res
