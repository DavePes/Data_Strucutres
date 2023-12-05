## Goal

The goal of this assignment is to experimentally evaluate Linear probing
hash table with different systems of hash functions.

You are given a test C++ program (`hash_experiment`) which implements everything
needed to perform the following experiments:

- _Grow experiment:_ This experiment tries different sizes $m$ of the hash table and for each size
  it inserts keys $1, 2, ..., 0.6\cdot m$ in this order (that is, the tables will be 60% full).
- _Usage experiment:_ This experiment uses hash table of size $2^{20}$. It performs insertions
  to increase usage of the table by 1%, reports efficiency of the insert operation,
  and repeats until usage of the table reaches 90%.

Both experiments measure the average number of probed buckets per operation, are repeated 100 times
and report the mean, standard deviation, and maximum of these averages over all repetitions.
Note that even with 100 (or more) repetitions the reported numbers still depend quite a lot on the random seed used.

You should perform these experiments for 4 different classes of hash functions –
tabulation, multiply-shift which uses low bits of upper half of 64-bit word (`ms-high`),
and polynomial hash function of degree 1 and 2 – and write a report, which contains three
plots of the measured data for each experiment, each plot with four curves. The first plot should contain average
complexity of operations over all repetitions, the second one the standard deviation, and the third one the maximum.

Each plot should show the dependence of the average number of probed buckets
either on size of the hash table (the grow experiment) or the usage of the hash table
(the usage experiment).

The report should discuss the experimental results and if possible, try to explain the observed
behavior using theory mentioned during the lecture. (If you want, you can carry out further
experiments to gain better understanding of the data structure and include these
in the report. This is strictly optional.)

You should submit a PDF file with the report (and no source code).
You will get 1 temporary point upon submission if the file is syntactically correct;
proper points will be assigned later.

## Test program

The test program is given two arguments:
- The name of the test (`{growSeq,usageSeq}-{ms-high,poly-1,poly-2,tab}`).
- The random seed: you should use the last 2 digits of your student ID (you can find
  it in the Study Information System – just click on the Personal data icon). Please
  include the random seed in your report.

The output of the program contains one line per experiment, which consists of
the table size (for growSeq) or usage of the table in percents (for usageSeq),
the mean of the averages, the standard deviation of the averages, and the maximum average
number of probes per insert.

Note that as Python tends to be substantially slower, the test program is provided in C++ only.
Nevertheless, to generate all the data needed for the plots,
it is sufficient to run `make` on a Linux machine with the `g++` compiler
(on Windows, one can use WSL, Cygwin, etc.).
Before running `make`, you only need to set the student ID inside `Makefile`.

## Hints

The following tools can be useful for producing nice plots:
- [pandas](https://pandas.pydata.org/)
- [matplotlib](https://matplotlib.org/)
- [gnuplot](http://www.gnuplot.info/)

A quick checklist for plots:
- Is there a caption explaining what is plotted?
- Are the axes clearly labelled? Do they have value ranges and units?
- Have you mentioned that this axis has logarithmic scale? (Logarithmic graphs
  are more fitting in some cases, but you should tell.)
- Is it clear which curve means what?
- Is it clear what are the measured points and what is an interpolated
  curve between them?
- Are there any overlaps? (E.g., the most interesting part of the curve
  hidden underneath a label?)

In your discussion, please distinguish the following kinds of claims.
It should be always clear which is which:
- Experimental results (i.e., the raw data you obtained from the experiments)
- Theoretical facts (i.e., claims have been proved mathematically)
- Your hypotheses (e.g., when you claim that the graph looks like something is true,
  but you are not able to prove rigorously that it always holds)

Source code templates can be found in [git](https://gitlab.kam.mff.cuni.cz/datovky/assignments/-/tree/master).
