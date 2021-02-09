#!/bin/bash



if [ "$1" = "server" ]
then
    if [ "$2" = "release" ]
    then
        ./bin/release/recty server
    else 
        ./bin/debug/recty server
    fi
elif [ "$1" = "client" ]
then
    if [ "$2" = "release" ]
    then
        ./bin/release/recty client
    else 
        ./bin/debug/recty client
    fi
else
    if [ "$2" = "release" ]
    then
        ./bin/release/recty
    else 
        ./bin/debug/recty
    fi
fi