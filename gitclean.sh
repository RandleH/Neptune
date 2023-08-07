#!/bin/bash


DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Clean working directory: ${DIR_CWD}"

cd ${DIR_CWD}
rm -rf CMakeCache.txt
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf build
rm -rf CMakeFiles
rm -rf model1.map


