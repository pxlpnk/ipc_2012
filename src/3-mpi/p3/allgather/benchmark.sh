#!/bin/bash

repeats=10

make clean
make

APP=p3-allgather.exe
data_dir=data
mkdir -p $data_dir

N=( 10 100 500 1000 5000 10000 50000 100000 )

PROCS=2
# NODES="-nn 1"
NODES=""
algos=( ref allgather )

# echo "test for fixed proc $PROCS and n in 10 100 500 1000 5000 10000 50000 100000"
# for I in $(seq 1 $repeats)
# do
#     for algo in "${algos[@]}"
#     do
#         for n in "${N[@]}"
#         do
#             data_file="${data_dir}/${algo}_fixed_procs_${PROCS}"
#             mpirun $NODES -np $PROCS $APP -a $algo -n $n -f $data_file
#         done
#     done
# done



N=10000

PROCS=2
# NODES="-nn 1"
NODES=""
algos=( ref allgather )

echo "test for fixed n $N and proc from 1..16"
for I in $(seq 1 $repeats)
do
    for algo in "${algos[@]}"
    do
        for p in $(seq 1 $PROCS)
        do
            data_file="${data_dir}/${algo}_variable_procs_${N}"
            mpirun $NODES -np $PROCS $APP -a $algo -n $n -f $data_file -i procs -p $p
        done
    done
done
