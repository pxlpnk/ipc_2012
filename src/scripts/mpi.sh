#!/bin/bash

. functions.sh

#function mpi-task-speedup () {
	#logfile="$GITROOT/data/$dir/speedup"
#
	#id=p
	#p=0
	#tries=10
#
	#if [ -n "$BENCHMARK" ]; then
		#alg="custom"
#
		#n=`print_power2 4`
		#run_it
	#fi
	#process
#}

function mpi-n () {
	id=n
	p=0
	tries=5
	#rnd_times=5
	#rnd_exp=28
	#rnd_seed=42

	algos="custom native seq"

	n="100000 `print_power2 24` `print_power2 26` `print_power2 28`"
	test-algos-n
}

dir="3-mpi/p2"
wd="$GITROOT/src/$dir"
if [ -n "$BENCHMARK" ]; then
	cd "$wd"
	make clean
	#make debug
	make fast

	#app="mpirun -np 576 -node 1-35  $wd/3-mpi-p2.exe"
	app="mpirun -np 560 -node 2-35  $wd/3-mpi-p2.exe"
	proc_opt="-p" # n/a
fi

#mpi-task-speedup
mpi-n

exit_func