#!/bin/sh
set -e

export NXDK_DIR=`pwd`
export PATH="${PATH}:${NXDK_DIR}/usr/bin:${NXDK_DIR}/usr/local/bin"

if [ $(uname) = 'Darwin' ]; then
    NUMCORES=$(sysctl -n hw.logicalcpu)
else
    NUMCORES=$(nproc)
fi

cd samples
for dir in */
do
    cd "$dir"
    make -j${NUMCORES}
    cd ..
done
