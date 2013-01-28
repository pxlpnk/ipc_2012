#!/bin/bash

. functions.sh

function openmp-n () {
	id=n
	p=0
	tries=10
	rnd_times=5
	rnd_exp=31
	rnd_seed=42

	#algos="seq reduction inplace auxarr hillis"
	algos="seq inplace auxarr hillis"

	# locally
	n=`print_power2_seq 18 20`
	# saturn
	#n=`print_power2_seq 18 26`' '`print_rand_seq`

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