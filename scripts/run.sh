#!/bin/bash



if [ "$1" = "server" ]
then
    if [ "$2" = "release" ]
    then
        ./bin/release/recty_server
    else 
        ./bin/debug/recty_server
    fi
else
    if [ "$2" = "release" ]
    then
        ./bin/release/recty
    else 
        ./bin/debug/recty
    fi
fi