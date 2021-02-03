#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

set -o xtrace

time ${DIR}/build/canny ${DIR}/../221575.pgm 0.5 0.7 0.9
${DIR}/build/validate ${DIR}/../221575-out-golden.pgm ${DIR}/../221575.pgm_s_0.50_l_0.70_h_0.90.pgm

