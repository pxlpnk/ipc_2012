# Summary

| Project                           | Andreas Code | Stefan Code | Benchmarks | Docs |
| --------------------------------- | ------------ | ----------- | ---------- | ---- |
| 2/3 projects, OpenMP OR pthreads  |              |             |            |      |
| OpenMP 1 prefix-sums              |              |     90%     |     0%     | 10%  |
| OpenMP 2 matrix-vector multiply   |     90%      |             |     0%     |  0%  |
| OpenMP 3 Parallel merge           |              |             |            |      |
| pthreads 1 prefix-sums            |              |             |            |      |
| pthreads 2 matrix-vector multiply |              |             |            |      |
| pthreads 3 Parallel merge         |              |             |            |      |
|                                   |              |             |            |      |
| 1/2 projects                      |              |             |            |      |
| Cilk 1 prefix-sums                |              |      0%     |            |      |
| Cilk 2 merge (bonus merge sort)   |              |             |            |      |
|                                   |              |             |            |      |
| 3/4 projects                      |              |             |            |      |
| MPI 1 stencil                     |              |             |            |      |
| MPI 2 prefix-sums                 |              |      0%     |            |      |
| MPI 3 matrix-vector multiply      |      0%      |             |            |      |
| MPI 4 bucket sort                 |              |             |            |      |

# Coding Guidelines, Style etc.
* Use uint for "normal" integer stuff like counters, indices etc.
* FIXME: stuff to fix before delivery
* TODO: woule be nice, but not mandatory

# Benchmarks & Plots
*	instanz-größen für jeden algo gleich, auch für unterschiedliche implementierungen/projekte,
	damit vergleichbarkeit möglich bleibt.
*	instanz-werte ebenfalls -> zentral, reproduzierbar generieren
*	welche instanz-größen und werte im allgemeinen?
	(auch) ungerade größen, ungefähr exponentiell ansteigen.
	mögliche wert(generatoren): counter, random, constant, best/worst case (pro algo, wie?)
*	welche hosts?
*	wieviele threads?
*	2D probleme sollen nach n und m geplottet werden

## plot generation
outputs nach `/data/#-<framework>/p<#>/` (wie /src)

*	output format der algos:
	TBD

*	input format für pgfplots:
	whitespace-separated values, `#` comment char, 1. line containing headers followed by the values (also whitespace-separated, line by line).

###	zeit/instanz-größe
		
	for each algo-implementation
		(avg, min, max, med)zeit/instanz-größe

input für pgfplots: file name: `size-<algoname>.plot`,
headers: `size min max avg med`

###	zeit/thread-anzahl
	for each algo-implementation
		(avg, min, max, med)zeit/thread-anzahl

input für pgfplots: file name: `threads-<algoname>.plot`,
headers: `threads min max avg med`

###	speedup/algo
	for each project
		speedup/algo

input für pgfplots: file name: `speedup.plot`,
headers: `algo speedup` where speedup is the median of all (selected) runs of an algorithm implementation

Alogorithm               | Instance Sizes | Instance Values |
-------------------------|--------------- | --------------- |
prefix-sums              |                |                 |
matrix-vector multiply   |                |                 |

## Additional requirements per project

*	1-1: reduction-clause vs. "full prefix-sums implementations":
	just another algo-implementation as far as plots are concerned
*	1-2: different schedules/cache sharing behaviors
*	1-2: placement of threads

# TODO

* 1-1: "summation can be implemented with a summation variable and a reduction-clause"
welche summation?

