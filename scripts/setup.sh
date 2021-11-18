#!/bin/bash

# script used incase ccache isn't behaving correctly with clion

[ ! - d "src" ] && echo "ERROR, src folder does not exist, try calling from root" && exit
rm build -rf
CC=clang CXX=clang++ meson setup build