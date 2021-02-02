#!/bin/bash

set -o xtrace

wget --no-clobber https://johnysswlab.com/downloads/221575.tar.gz
tar -xf 221575.tar.gz

CC=clang
CFLAGS="-O3 -ffast-math -march=core-avx2 -lm"

${CC} ${CFLAGS}  canny_source.c -o canny

time ./canny 221575.pgm 0.5 0.7 0.9
