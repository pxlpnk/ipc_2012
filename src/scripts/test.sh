#!/bin/bash

function print_power2_seq () { # print 2^$1 till 2^$2 (inclusive)
	i=`awk "BEGIN {print 2^$1}"`
	end=`awk "BEGIN {print 2^$2}"`
	while [[ $i -le $end ]]; do
		echo -n "$i "
		i=$(($i*2))
	done
}

function test_it () {
	if [ -z "$tries" -o -z "$app" -o -z "$proc_opt" -o -z "$algos" -o -z "$p" -o -z "$n" -o -z "$logfile" ]; then
		echo "missing parameter"
		exit 1
	fi
	for alg in $algos; do
		for N in $n; do
			for NPROC in $p; do
				cli="$app $proc_opt $NPROC -a $alg -n $N -f $logfile -i $id"
				echo "Testing '$cli' $tries times"
				for x in `seq 1 $tries`; do
					$cli
					#echo "$cli"
					bla=$?
					if [[ ! $bla -eq 0 ]]; then
						echo "'$cli' failed: $bla"
						exit $bla
					fi
				done
			done
		done
	done
}

function process () {
	if [ -z "$SCRIPTDIR" -o -z "$logfile" ]; then
		echo "missing parameter"
		exit 1
	fi
	echo -n "processing '"$logfile"'... "
	"$SCRIPTDIR/stats.rb" "$logfile" > "$logfile".processed
	bla=$?
	if [[ ! $bla -eq 0 ]]; then
		echo "'"$SCRIPTDIR/stats.rb" "$logfile"' failed: $bla"
		exit $bla
	fi
	echo "done"
}

function cilk () {
	dir="2-cilk/p1"
	logfile="$GITROOT/data/$dir/data-n"
	wd="$GITROOT/src/$dir"
	if [ -n "$BENCHMARK" ]; then
		cd "$wd"
		make clean
		make fast

		app="$wd/2-cilk-p1.exe"
		proc_opt="--nproc"

		tries=10
		algos="data"
		p=0
		#n=`print_power2_seq 2 4`
		n=`seq 1 32`' '`print_power2_seq 5 8`
		id=n
		test_it
	fi

	process
	#algos="data task"
	#p=`seq 1 4`
	#n=`seq 1 32`' '`print_power2_seq 5 8`
}

# http://stackoverflow.com/a/246128/1905491
OCWD=`pwd`
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TIMESTAMP=`date "+%Y%m%d-%H%M"`
GITROOT=`git rev-parse --show-toplevel`

BENCHMARK=
while getopts "b" OPTION ; do
	case $OPTION in
	  #t)
		  #TEST=$OPTARG
		  #;;
	  b)
		  BENCHMARK=BENCHMARK
		  echo "Will run benchmarks"
		  ;;
	esac
done

cilk 

cd $OCWD
echo "done!"
exit 0

done