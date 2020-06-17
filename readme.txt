This repository contains the test instances we generated for our computational experiments using our MILP models and the Breakout Local Search algorithm for the "The rank one quadratic assignment problem". The paper containing these experimental results will be published in INFORMS Journal on Computing which also provide further details about the models, algorithm, and experimental analysis. There is also a supplementary page to be published by the journal with additional details. We will provide a link to these resourses here, once they are available. The repository also contains the best solution obtained for each of the instances by our MILP models (run under specified time limits) and the Breakout Local Search algorithm.

The name of each instance is represented in the format QR1-n-xxx, where n is the problem size. The data file is arranged as follows:

line 1: the size n of the instance
next n lines: the matrix A (read row wise)
the following n lines: the matrix B (read row wise)
and the remaining n lines: the matrix C (read row wise)

The best solution file for each instance is organized as follows:

line 1: the objective function value of the best solution
line 3: a permutation of 1, 2, ..., n representing the best solution obtained.
