
"Random_generator.cpp" is used to generated the test case of "QR1-xxx-S" and "QR1-xxx-L". "XXX" is the size of the matrix.

In random test case:
    S(small range): Each of the entries in matrix a are limited to [1-10], matrix b are limited to [10-20] and matrix c are limited to [20-40] 
    within: a_lower_bound = 1;a_upper_bound = 10;b_lower_bound = 10;b_upper_bound = 20;c_lower_bound = 20;c_upper_bound = 40;

    L(large range): Each of the entries in matrix a are limited to [1-50], matrix b are limited to [50-100] and matrix c are limited to[100-400]
    within: a_lower_bound = 1;a_upper_bound = 50;b_lower_bound = 50;b_upper_bound = 100;c_lower_bound = 100;c_upper_bound = 400;

QR1-Rgen_generator.cpp is used to generated the test case of "QR1-xxx-Rgen-S" and "QR1-xxx-Rgen-L". 
note: In this code, the original test case comes from random test case. 

QR1-XCRow_generator.cpp is used to generated the test case of "QR1-xxx-NCR-L", "QR1-xxx-PCR-L", "QR1-xxx-NCR-S" and "QR1-xxx-PCR-S". "X" means "P" or "N". 
note: In this code, the original test case comes from random test case.

QR1-XCVct_generator.cpp is used to generated the test case of "QR1-xxx-NCV-L", "QR1-xxx-PCV-L", "QR1-xxx-NCV-S" and "QR1-xxx-PCV-S". "X" means "P" or "N". 
note: In this code, the original test case comes from random test case.

All seed values that involved are default values of "1".
