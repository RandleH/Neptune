#!/bin/bash


DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Clean working directory: ${DIR_CWD}"

cd ${DIR_CWD}
rm -rf build





