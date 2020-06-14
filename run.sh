#!/bin/bash

BINARY="taskset -c 0 ./build/LF_Codec"
DATASET=~/lfcodec/datasets/Bikes/
RESULT_DCT=../results-single/DCT/
RESULT_DST_II=../results-single/DST_II/
RESULT_DST_VII=../results-single/DST_VII/
RESULT_DST_VII_2=../results-single/DST_VII_2/
RESULT_FWHT=../results-single/FWHT/

QP=1
AXIS_X=1
AXIS_Y=2
AXIS_U=4
AXIS_V=8
FLIPS=0 #$((AXIS_X))
SEGMENTS=0

mkdir -p $RESULT_DCT
mkdir -p $RESULT_DST_II
mkdir -p $RESULT_DST_VII
# mkdir -p $RESULT_DST_VII_2

# valgrind --leak-check=yes\
# gdb --args \
$BINARY -input $DATASET -output $RESULT_DCT \
    -blx 15 -bly 15 -blu 13 -blv 13 \
    -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP \
    -lfx 625 -lfy 434 -lfu 13 -lfv 13 \
    -transform DCT -segment $SEGMENTS -flipaxis $FLIPS

$BINARY -input $DATASET -output $RESULT_DST_II \
    -blx 15 -bly 15 -blu 13 -blv 13 \
    -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP \
    -lfx 625 -lfy 434 -lfu 13 -lfv 13 \
    -transform DST_II -segment $SEGMENTS -flipaxis $FLIPS

# $BINARY -input $DATASET -output $RESULT_DST_VII \
#     -blx 15 -bly 15 -blu 13 -blv 13             \
#     -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP     \
#     -lfx 625 -lfy 434 -lfu 13 -lfv 13           \
#     -transform DST_VII  -segment $SEGMENTS -flipaxis $FLIPS


# $BINARY -input $DATASET -output $RESULT_FWHT \
#     -blx 15 -bly 15 -blu 13 -blv 13             \
#     -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP     \
#     -lfx 625 -lfy 434 -lfu 13 -lfv 13           \
#     -transform FWHT  -segment $SEGMENTS -flipaxis $FLIPS

# $BINARY -input $DATASET -output $RESULT_DST_VII_2 \
#     -blx 15 -bly 15 -blu 13 -blv 13 \
#     -qx $QP -qy $QP -qu $QP -qv $QP -qp $QP \
#     -lfx 625 -lfy 434 -lfu 13 -lfv 13 \
#     -transform DST_VII_2 -segment $SEGMENTS -flipaxis $FLIPS
