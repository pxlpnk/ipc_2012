#!/bin/bash

repeats=10

make clean
make

data_dir=data
mkdir -p $data_dir

N=1000
M=( 10 100 500 1000 5000 10000 50000 100000 )
PROCS=2
algos=( ref false_sharing tiling )

echo "test for fixed proc $PROCS fixed n $N and m in 10 100 500 1000 5000 10000 50000 100000"
for I in $(seq 1 $repeats)
do
    for algo in "${algos[@]}"
    do
        for m in "${M[@]}"
        do
            data_file="${data_dir}/${algo}_fixed_n_${N}"
            ./p2-matrix_vector_openmp.exe -p $PROCS -a $algo -m $m -n $N -f $data_file
        done
    done
done


M=1000
N=( 10 100 500 1000 5000 10000 50000 100000 )
PROCS=2
algos=( ref false_sharing tiling )

echo "test for fixed proc $PROCS fixed m $M and n in 10 100 500 1000 5000 10000 50000 100000"
for I in $(seq 1 $repeats)
do
    for algo in "${algos[@]}"
    do
        for n in "${N[@]}"
        do
            data_file="${data_dir}/${algo}_fixed_m_${M}"
            ./p2-matrix_vector_openmp.exe -p $PROCS -a $algo -m $m -n $N -f $data_file
        done
    done
done


M=1000
N=100000
PROCS=2
algos=( ref false_sharing tiling )

echo "test for proc 1 .. $PROCS fixed m $M and fixed n $N"
for I in $(seq 1 $repeats)
do
    for algo in "${algos[@]}"
    do
        for p in $(seq 1 $PROCS)
        do
            data_file="${data_dir}/${algo}_variable_procs_${M}x${N}"
            ./p2-matrix_vector_openmp.exe -p $p -a $algo -m $M -n $N -i procs -f $data_file
        done
    done
done


N=1000
M=100000
PROCS=2
algos=( ref false_sharing tiling )

echo "test for proc 1 .. $PROCS fixed m $M and fixed n $N"
for I in $(seq 1 $repeats)
do
    for algo in "${algos[@]}"
    do
        for p in $(seq 1 $PROCS)
        do
            data_file="${data_dir}/${algo}_variable_procs_${M}x${N}"
            ./p2-matrix_vector_openmp.exe -p $p -a $algo -m $M -n $N -i procs -f $data_file
        done
    done
done
