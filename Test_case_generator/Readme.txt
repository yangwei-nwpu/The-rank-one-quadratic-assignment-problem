
"Random_generator.cpp" is used to generated the test case of "QR1-xxx-R-S" and "QR1-xxx-R-L". "XXX" is the size of the matrix.

In random test case£º
    S(small range)£ºEach of the entries in matrix a are limited to [1-10], matrix b are limited to [10-20] and matrix c are limited to [20-40] 
    within£º a_lower_bound = 1;a_upper_bound = 10;b_lower_bound = 10;b_upper_bound = 20;c_lower_bound = 20;c_upper_bound = 40;

    L(large range):  Each of the entries in matrix a are limited to [1-50], matrix b are limited to [50-100] and matrix c are limited to[100-400]
    within£º a_lower_bound = 1;a_upper_bound = 50;b_lower_bound = 50;b_upper_bound = 100;c_lower_bound = 100;c_upper_bound = 400;

QR1-Rgen_generator.cpp is used to generated the test case of "QR1-xxx-Rgen-S" and "QR1-xxx-Rgen-L". 
In this code, the original test case comes from random test case. 

QR1-XCRow_generator.cpp is used to generated the test case of "QR1-xxx-R-NCRow-L", "QR1-xxx-R-PCRow-L", "QR1-xxx-R-NCRow-S" and "QR1-xxx-R-PCRow-S". "X" means "P" or "N". 
In this code, the original test case comes from random test case.

QR1-XCVct_generator.cpp is used to generated the test case of "QR1-xxx-R-NCVct-L", "QR1-xxx-R-PCVct-L", "QR1-xxx-R-NCVct-S" and "QR1-xxx-R-PCVct-S". "X" means "P" or "N". 
In this code, the original test case comes from random test case.

All seed values that involved are default values of "1".
