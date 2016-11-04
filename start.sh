#! /usr/bin/bash
set -o errexit # остановка после первой ошибки
export PATH=/home/vadim/mpich-install/bin:$PATH
make all
mpiexec -n 9 build/bin/solve 1>data/result.txt