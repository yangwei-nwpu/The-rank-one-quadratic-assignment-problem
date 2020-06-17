We upload the problem instances studied in the Informs Journal on Computing paper "The rank one quadratic assignment problem" 
and the best solution for each instance obtained by the MILP models and Breakout Local Search algorithms. 

The format of each problem instance QR1-n-xxx is organized as follows:

line 1: the size n of the instance
the following n lines: the matrix A
the following n lines: the matrix B
the following n lines: the matrix C

The format of each best solution file is organized as follows:

line 1: the objective function value of the best solution
line 3: a permutation of 1, 2, ..., n to represent the best solution obtained by solving this instance. 
