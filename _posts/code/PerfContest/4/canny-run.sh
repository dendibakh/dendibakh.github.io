#!/bin/bash

set -o xtrace

wget --no-clobber https://johnysswlab.com/downloads/221575.tar.gz
tar -xf 221575.tar.gz

CC=clang
CFLAGS="-O3 -ffast-math -march=core-avx2 -lm"
CXXFLAGS="${CFLAGS} -lstdc++"

${CC} ${CFLAGS}  canny_source.c -o canny
${CC} ${CXXFLAGS} validate.cpp -o validate

time ./canny 221575.pgm 0.5 0.7 0.9
./validate 221575.pgm_s_0.50_l_0.70_h_0.90.pgm.orig 221575.pgm_s_0.50_l_0.70_h_0.90.pgm