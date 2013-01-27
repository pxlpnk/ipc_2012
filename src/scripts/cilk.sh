#!/bin/bash

. functions.sh

function cilk-task-speedup () {
	logfile="$GITROOT/data/$dir/task-speedup"

	id=p
	p=0
	tries=10

	if [ -n "$BENCHMARK" ]; then
		algos="task"

		n=`print_power2_seq 20 20`
		p=`seq 1 48`' '64
		test_it
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

	if [ -n "$BENCHMARK" ]; then
		algos="data"

		n=`print_power2_seq 2 4`
		#n=`print_power2_seq 1 26`' '
		#n=`seq 1 32`' '`print_power2_seq 5 31`
		test_it

		#n=`print_rand_seq`
		#test_it
	fi
	process

	logfile="$GITROOT/data/$dir/task-n"
	if [ -n "$BENCHMARK" ]; then
		algos="task"
		n=`print_power2_seq 2 4`
		#n=`print_power2_seq 1 26`' '
		#n=`seq 1 32`' '`print_power2_seq 5 31`
		test_it

		#n=`print_rand_seq`
		#test_it
	fi
	process
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