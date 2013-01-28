#!/bin/bash

. functions.sh

function cilk-task-speedup () {
	logfile="$GITROOT/data/$dir/task-speedup"

	id=p
	tries=10

	if [ -n "$BENCHMARK" ]; then
		alg="task"

		p=`seq 1 48`' '64
		n=`print_power2 20`
		run_it
	fi
	process
}

function cilk-n () {
	logfile="$GITROOT/data/$dir/data-n"

	id=n
	p=0
	tries=10
	rnd_times=5
	rnd_exp=28
	rnd_seed=42

	algos="data task"

	n=`print_power2_seq 2 4`
	#n=`print_power2_seq 1 26`' '`print_rand_seq`
	#n=`seq 1 32`' '`print_power2_seq 5 31`
	test-algos-n
}

dir="2-cilk/p1"
wd="$GITROOT/src/$dir"
if [ -n "$BENCHMARK" ]; then
	cd "$wd"
	make clean
	make fast

	app="$wd/2-cilk-p1.exe"
	proc_opt="--nproc"
fi

cilk-task-speedup
cilk-n

exit_func