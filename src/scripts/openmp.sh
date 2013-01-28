#!/bin/bash

. functions.sh

function openmp-n () {
	id=n
	p=0
	tries=10
	rnd_times=5
	rnd_exp=28
	rnd_seed=42

	#algos="seq reduction inplace auxarr hillis"
	algos="seq inplace auxarr"

	n=`print_power2_seq 18 20`
	#n=`print_power2_seq 18 26`' '`print_rand_seq`
	#n=`seq 1 32`' '`print_power2_seq 5 31`
	test-algos-n
}

dir="1-openmp/p1"
wd="$GITROOT/src/$dir"
if [ -n "$BENCHMARK" ]; then
	cd "$wd"
	make clean
	make fast

	app="$wd/1-openmp-p1.exe"
	proc_opt="--nproc"
fi

openmp-n

exit_func