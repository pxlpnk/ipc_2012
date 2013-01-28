#!/bin/bash

. functions.sh

function cilk-task-speedup () {
	logfile="$GITROOT/data/$dir/task-speedup"

	id=p

	if [ -n "$BENCHMARK" ]; then
		alg="task"

		# locally
		p=`seq 1 4`
		n=`print_power2 20`
		# saturn
		#p="1 2 `seq 3 3 48` 64"
		#n=2147483648
		run_it
	fi
	process
}

function cilk-n () {
	logfile="$GITROOT/data/$dir/data-n"

	id=n
	p=0
	rnd_times=5
	rnd_exp=28
	rnd_seed=42

	algos="data task"

	# locally
	n=`print_power2_seq 10 12`
	# saturn
	#n="`print_power2_seq 1 2 26` `print_rand_seq`"
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
	tries=6
fi

cilk-task-speedup
cilk-n

exit_func
