#!/bin/bash

gdb ./bin/debug/recty 

if [ "$1" = "server" ]
then
    gdb ./bin/debug/recty_server 
else
    gdb ./bin/debug/recty 
fi