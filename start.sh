#! /usr/bin/bash
set -o errexit # остановка после первой ошибки
export PATH=/home/vadim/mpich-install/bin:$PATH
mkdir -p data/results
>data/results/test.txt
make all
for((i=2; i < 30; i++)); do
	prog_time="$i, $(mpiexec -n $i build/bin/solve 20000 40000 2>&1 1>data/result.txt)"
	echo $prog_time >>data/results/test.txt
	echo $prog_time
done
cat data/results/test.txt