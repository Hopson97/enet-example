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

if [ -d "$recty" ]
then
    rm -rf -d recty
fi

mkdir -p recty

mkdir -p bin
cd bin 
mkdir -p release
mkdir -p debug

if [ "$1" = "release" ]
then
    target_release
    cp bin/release/recty recty/recty
    cp bin/release/recty_server recty/recty_server
else
    target_debug
    cp bin/debug/recty recty/recty
    cp bin/debug/recty_server recty/recty_server
fi

cp -R res recty/res
