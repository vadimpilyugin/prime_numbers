#! /usr/bin/bash
set -o errexit # остановка после первой ошибки
export PATH=/home/vadim/mpich-install/bin:$PATH
mkdir -p data/results
mkdir -p data/pictures
:>data/results/sum.txt
:>data/results/max.txt
make all
for((i=1; i < 2; i++)); do
	prog_time=$(mpiexec -n $i build/bin/solve 1 10000000 2>&1 1>data/result.txt)
	time=($prog_time)
	time_all="$i, ${time[1]}"
	time_max="$i, ${time[0]}"
	echo "$i $prog_time"
	echo $time_all >>data/results/sum.txt
	echo $time_max >>data/results/max.txt
done 
gnuplot plot/plot_max.gpl
gnuplot plot/plot.gpl