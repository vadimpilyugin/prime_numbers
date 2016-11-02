#! /usr/bin/bash
set -o errexit # остановка после первой ошибки
export PATH=/home/vadim/mpich-install/bin:$PATH
make all
mpiexec -n 3 build/bin/solve