#!/bin/bash



if [ "$1" = "server" ]
then
    if [ "$2" = "release" ]
    then
        ./bin/release/recty server
    else 
        ./bin/debug/recty server
    fi
else
    if [ "$2" = "release" ]
    then
        ./bin/release/recty client
    else 
        ./bin/debug/recty client
    fi
fi