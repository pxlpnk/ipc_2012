#!/bin/bash

function print_power2_seq () { # print 2^$1 till 2^$2 (inclusive)
	i=`awk "BEGIN {print 2^$1}"`
	end=`awk "BEGIN {print 2^$2}"`
	while [[ $i -le $end ]]; do
		echo -n "$i "
		i=$(($i*2))
	done
}

function print_rand_seq () { # print $rnd_times times 2^<random> <= 2^$rnd_exp, seeded by $rnd_seed
	if [ -z "$rnd_seed" -o -z "$rnd_times" -o -z "$rnd_exp" ]; then
		echo "missing parameter"
		exit 1
	fi
	RANDOM=$rnd_seed
	limit=1
	for i in `seq 1 $rnd_exp`; do
		limit=$(($limit*2))
	done
	for i in `seq 1 $rnd_times`; do
		number=$RANDOM
		while [[ $number -le $limit ]]; do
			number=$(($number*2))
		done
		let "number %= $limit"
		echo -n "$number "
	done
}

function test_it () {
	if [ -z "$tries" -o -z "$app" -o -z "$proc_opt" -o -z "$algos" -o -z "$p" -o -z "$n" -o -z "$logfile" ]; then
		echo "missing parameter"
		exit 1
	fi
	if [ -n "$CLEAN" ]; then
		rm -f "$logfile"
	fi
	mkdir -p `dirname "$logfile"`
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
	if [ -n "$CLEAN" ]; then
		rm -f "$logfile.processed"
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

OCWD=`pwd`
# http://stackoverflow.com/a/246128/1905491
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TIMESTAMP=`date "+%Y%m%d-%H%M"`
GITROOT=`git rev-parse --show-toplevel`

function usage () {
cat << EOF
usage: $0 [-h | [-b] [-c]]

This script runs benchmarks and processes the results

OPTIONS:
    -h      Show this message
    -b      Run the benchmarks
    -c      Clean previous logs that will be regenerated later in this run
EOF
}

BENCHMARK=
while getopts ":bch" OPTION ; do
	case $OPTION in
	'?')
		echo  ------------------------
		echo "Error: unknown option: $OPTARG"
		echo  ------------------------
		echo
		usage
		exit 1
		;;
	h)
		usage
		exit 0
		;;
	#t)
		  #TEST=$OPTARG
		  #;;
	b)
		  BENCHMARK=BENCHMARK
		  echo "Will run benchmarks"
		  ;;
	c)
		  CLEAN=CLEAN
		  echo "Will clean previous logs"
		  ;;
	esac
done

function exit_func () {
	cd $OCWD
	echo "done!"
	exit 0
}