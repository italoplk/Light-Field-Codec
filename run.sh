#!/bin/bash

BINARY="taskset -c 0 ./build/LF_Codec"
DATASET=~/lfcodec/datasets/Bikes/
RESULT_DCT=../results-single/DCT/
RESULT_DST_II=../results-single/DST_II/
RESULT_DST_VII=../results-single/DST_VII/
QP=1

mkdir -p $RESULT_DCT
mkdir -p $RESULT_DST_II
mkdir -p $RESULT_DST_VII

# valgrind --leak-check=yes\
# gdb --args \
$BINARY -input $DATASET -output $RESULT_DCT \
    -blx 15 -bly 15 -blu 13 -blv 13             \
    -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP     \
    -lfx 625 -lfy 434 -lfu 13 -lfv 13           \
    -transform DCT    

$BINARY -input $DATASET -output $RESULT_DST_II \
    -blx 15 -bly 15 -blu 13 -blv 13             \
    -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP     \
    -lfx 625 -lfy 434 -lfu 13 -lfv 13           \
    -transform DST_II    

# $BINARY -input $DATASET -output $RESULT_DST_VII \
#     -blx 15 -bly 15 -blu 13 -blv 13             \
#     -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP     \
#     -lfx 625 -lfy 434 -lfu 13 -lfv 13           \
#     -transform DST_VII


