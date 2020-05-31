#include <ilcplex/ilocplex.h>//cplex
#include <iostream>//std
#include <stdlib.h>//srand,rand
#include <time.h>//clock_t
#include <iomanip>//table display
#include <math.h>//log2

using namespace std;
//fucntion prototype
void optimization_of_quadratic_transpotation(int);//begining of the the program
int maximizeB(int**, int);//get the value of u by maximize B(X);
int minimizeB(int**, int);//get the value of l by minimize B(X);
int maximizeA(int**, int);//get the value of uKnot by maximize A(X);
int minimizeA(int**, int);//get the value of lKnot by minimize A(X);
						  //int log2(double);//get the logarithm of value in base 2
void solve(int**, int**, int**, int, int);//solving the quadratic transportation problem
										  //end of function prototype
										  //------------------------------variable declaration----------------------------------//
int n = 55;
int seed = 1;
double minimum_execution_time = DBL_MAX;
double maximum_execution_time = DBL_MIN;
double total_execution_time = 0.0;
double timea = 0.0;
double timeaa = 0.0;
double timeb = 0.0;
double timebb = 0.0;
double timec = 0.0;
double timecc = 0.0;
double timed = 0.0;
double timedd = 0.0;
bool isOptimal = true;
int* current;
const char separator = ' ';
const int nameWidth = 8;
//------------------------------end of variable declaration--------------------------//
char * textfilename = "C:/Users/wei yang/Desktop/QAP/test2/QR1-200-R-S.txt";
char * resultfile = "C:/Users/wei yang/Desktop/QAP/ceshicode/QR1-200-R-S.txt";
char * logfile = "C:/Users/wei yang/Desktop/QAP/ceshicode/logQR1-200-R-S.txt";
//char * sol = "C:/Users/wei yang/Desktop/test/test10/QR1-Rgen-S/milp2s/solutionfileQR1-1600-R-PCVct-S";
int main()
{
	//-----------------begining the test of 5 different problems by 5 different problem sizes----------------------------------//
	int count = 0;

	cout << "Followings are the result for MILP3" << endl;
	cout << endl;
	cout << left << setw(nameWidth) << setfill(separator) << "Test";
	cout << left << setw(nameWidth) << setfill(separator) << "n";
	cout << left << setw(nameWidth) << setfill(separator) << "n";
	cout << left << setw(20) << setfill(separator) << "Objective value";
	cout << left << setw(13) << setfill(separator) << "Time(s)";
	cout << left << setw(nameWidth) << setfill(separator) << "Result";
	cout << endl;

	while (count < 1)
	{
		optimization_of_quadratic_transpotation(count);
		count++;
	}

	cout << endl;
	cout << "Minimum Execution Time: " << minimum_execution_time << endl;
	cout << "Maximum Execution Time: " << maximum_execution_time << endl;
	cout << fixed;
	cout << "Average Execution Time: " << total_execution_time / 2 << endl;
	cout << endl;
	cout << endl;
	FILE *fp;
	errno_t err;
	err = fopen_s(&fp, resultfile, "a+");
	fprintf(fp, "Minimum Execution Time:%f\n", minimum_execution_time);
	fprintf(fp, "Maximum Execution Time:%f\n", maximum_execution_time);
	fprintf(fp, "Average Execution Time:%f\n", total_execution_time / 1);
	fclose(fp);
	minimum_execution_time = DBL_MAX;
	maximum_execution_time = DBL_MIN;
	total_execution_time = 0.0;
	count = 0;
	system("pause");
	return 0;
}
void optimization_of_quadratic_transpotation(int count)
{
	int** a;//n x n
	int** b;//n x n
	int** c;//n x n
	long object;
	ifstream ReadFile;
	string temp;
	ReadFile.open(textfilename, ios::in);
	ReadFile >> n;
	ReadFile >> object;
	cout << n << " " << object << endl;
	//n += 10;
	//------------------------------------------------instantiating a,b,c(two dimensional) and l,u(constant)---------------------------------------//	
	a = new int*[n];
	b = new int*[n];
	c = new int*[n];

	for (int i = 0; i < n; i++)
	{
		a[i] = new int[n];
		b[i] = new int[n];
		c[i] = new int[n];
	}
	for (int i = 0; i <n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> a[i][j];
		}
	}
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> b[i][j];
		}
	}
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> c[i][j];
		}
	}
	ReadFile.close();

	solve(a, b, c, n, count);
	//de-allocate heap memory
	for (int i = 0; i < n; i++)
	{
		delete[] a[i];
		delete[] b[i];
		delete[] c[i];
	}

	delete[] a;
	delete[] b;
	delete[] c;
}

// Solving the A(X) 
// PRE: 
// PARAM: a - two dimensional array, n - # of rows, n - # of columns
// POST:  objective cost of A(X) is returned
int maximizeA(int** a, int n)
{
	IloEnv env;
	try {
		IloModel model(env);

		/*begin of instantiating objective variables where
		variables are presented in two dimensional array
		and each variable such Xij is {0,1}*/
		IloArray<IloBoolVarArray> x(env, n);
		for (int i = 0; i < n; i++)
		{
			x[i] = IloBoolVarArray(env, n);
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				x[i][j] = IloBoolVar(env);
			}
		}//end of Xij
		 //-----------------------end of instantiating objective variables-------------------------------//


		 //--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum += a[i][j] * x[i][j];
			}
		}

		model.add(IloMaximize(env, linear_sum));
		linear_sum.end();
		//------------------------------end of objective function----------------------------------------//

		/*begin of instantiating contraints where constraint1
		is the sum of x[i][j] from j = 1 to n that is equal to 1,
		i = 1 to n and constraint2 is the sum of of x[i][j]
		from i = 1 to n that is equal to 1;*/
		for (int i = 0; i < n; i++)
		{
			IloNumExpr constraint1(env);
			for (int j = 0; j < n; j++)
			{
				constraint1 += x[i][j];
			}
			model.add(constraint1 == 1);
			constraint1.end();
		}

		for (int j = 0; j < n; j++)
		{
			IloNumExpr constraint2(env);
			for (int i = 0; i < n; i++)
			{
				constraint2 += x[i][j];
			}
			model.add(constraint2 == 1);
			constraint2.end();
		}
		//-------------------------------------------end of constraints of variable Xij--------------------------//

		//------------------------------------------------end of contriants-------------------------------------//

		//begin of solving 
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, fabs(3600.000000000000));
		clock_t begin = clock();
		cplex.solve();
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}

		int objectiveValue = cplex.getObjValue();
		clock_t end = clock();
		timea = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "ax上界求解时间：" << timea << endl;
		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
		}

		x.end();
		cplex.end();
		clock_t end1 = clock();
		timeaa = (double)(end1 - begin) / CLOCKS_PER_SEC;
		return objectiveValue;

	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
		abort();
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
		abort();
	}

	env.end();

}

// Solving the A(X) 
// PRE: 
// PARAM: a - two dimensional array, n - # of rows, n - # of columns
// POST:  objective cost of A(X) is returned
int minimizeA(int** a, int n)
{
	IloEnv env;
	try {
		IloModel model(env);

		/*begin of instantiating objective variables where
		variables are presented in two dimensional array
		and each variable such Xij is {0,1}*/
		IloArray<IloBoolVarArray> x(env, n);
		for (int i = 0; i < n; i++)
		{
			x[i] = IloBoolVarArray(env, n);
		}
		
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				x[i][j] = IloBoolVar(env);
			}
		}//end of Xij
		 //-----------------------end of instantiating objective variables-------------------------------//


		 //--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum += a[i][j] * x[i][j];
			}
		}

		model.add(IloMinimize(env, linear_sum));
		linear_sum.end();
		//------------------------------end of objective function----------------------------------------//

		/*begin of instantiating contraints where constraint1
		is the sum of x[i][j] from j = 1 to n that is equal to 1,
		i = 1 to n and constraint2 is the sum of of x[i][j]
		from i = 1 to n that is equal to 1;*/
		for (int i = 0; i < n; i++)
		{
			IloNumExpr constraint1(env);
			for (int j = 0; j < n; j++)
			{
				constraint1 += x[i][j];
			}
			model.add(constraint1 == 1);
			constraint1.end();
		}

		for (int j = 0; j < n; j++)
		{
			IloNumExpr constraint2(env);
			for (int i = 0; i < n; i++)
			{
				constraint2 += x[i][j];
			}
			model.add(constraint2 == 1);
			constraint2.end();
		}
		//-------------------------------------------end of constraints of variable Xij--------------------------//

		//------------------------------------------------end of contriants-------------------------------------//

		//begin of solving 
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, fabs(3600.000000000000));
		clock_t begin = clock();
		cplex.solve();

		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}

		int objectiveValue = cplex.getObjValue();
		clock_t end = clock();
		timeb = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "ax下界求解时间" << timeb << endl;
		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();
		cplex.end();
		clock_t end1 = clock();
		timebb = (double)(end1 - begin) / CLOCKS_PER_SEC;
		return objectiveValue;

	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
		abort();
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
		abort();
	}

	env.end();
}

// Solving the B(X) 
// PRE: 
// PARAM: b - two dimensional array, n - # of rows, n - # of columns
// POST:  objective cost of B(X) is returned and the optimal solution is stored in xPrime
int maximizeB(int** b, int n)
{
	IloEnv env;
	try {
		IloModel model(env);

		/*begin of instantiating objective variables where
		variables are presented in two dimensional array
		and each variable such Xij is {0,1}*/
		IloArray<IloBoolVarArray> x(env, n);
		for (int i = 0; i < n; i++)
		{
			x[i] = IloBoolVarArray(env, n);
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				x[i][j] = IloBoolVar(env);
			}
		}//end of Xij
		 //-----------------------end of instantiating objective variables-------------------------------//


		 //--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum += b[i][j] * x[i][j];
			}
		}

		model.add(IloMaximize(env, linear_sum));
		linear_sum.end();
		//------------------------------end of objective function----------------------------------------//

		/*begin of instantiating contraints where constraint1
		is the sum of x[i][j] from j = 1 to n that is equal to 1,
		i = 1 to n and constraint2 is the sum of of x[i][j]
		from i = 1 to n that is equal to 1;*/
		for (int i = 0; i < n; i++)
		{
			IloNumExpr constraint1(env);
			for (int j = 0; j < n; j++)
			{
				constraint1 += x[i][j];
			}
			model.add(constraint1 == 1);
			constraint1.end();
		}

		for (int j = 0; j < n; j++)
		{
			IloNumExpr constraint2(env);
			for (int i = 0; i < n; i++)
			{
				constraint2 += x[i][j];
			}
			model.add(constraint2 == 1);
			constraint2.end();
		}
		//-------------------------------------------end of constraints of variable Xij--------------------------//

		//------------------------------------------------end of contriants-------------------------------------//

		//begin of solving 
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, fabs(3600.000000000000));
		clock_t begin = clock();
		cplex.solve();
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}

		int objectiveValue = cplex.getObjValue();
		clock_t end = clock();
		timec = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "bx上界求解时间：" << timec << endl;
		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();
		//model.end();
		cplex.end();
		clock_t end1 = clock();
		timecc = (double)(end1 - begin) / CLOCKS_PER_SEC;
		return objectiveValue;

	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
		abort();
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
		abort();
	}

	env.end();
}

// Solving the B(X) 
// PRE: 
// PARAM: b - two dimensional array, n - # of rows, n - # of columns
// POST:  objective cost of B(X) is returned
int minimizeB(int** b, int n)
{
	IloEnv env;
	try {
		IloModel model(env);

		/*begin of instantiating objective variables where
		variables are presented in two dimensional array
		and each variable such Xij is {0,1}*/
		IloArray<IloBoolVarArray> x(env, n);
		for (int i = 0; i < n; i++)
		{
			x[i] = IloBoolVarArray(env, n);
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				x[i][j] = IloBoolVar(env);
			}
		}//end of Xij
		 //-----------------------end of instantiating objective variables-------------------------------//


		 //--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum += b[i][j] * x[i][j];
			}
		}

		model.add(IloMinimize(env, linear_sum));
		linear_sum.end();
		//------------------------------end of objective function----------------------------------------//

		/*begin of instantiating contraints where constraint1
		is the sum of x[i][j] from j = 1 to n that is equal to 1,
		i = 1 to n and constraint2 is the sum of of x[i][j]
		from i = 1 to n that is equal to 1;*/
		for (int i = 0; i < n; i++)
		{
			IloNumExpr constraint1(env);
			for (int j = 0; j < n; j++)
			{
				constraint1 += x[i][j];
			}
			model.add(constraint1 == 1);
			constraint1.end();
		}

		for (int j = 0; j < n; j++)
		{
			IloNumExpr constraint2(env);
			for (int i = 0; i < n; i++)
			{
				constraint2 += x[i][j];
			}
			model.add(constraint2 == 1);
			constraint2.end();
		}
		//-------------------------------------------end of constraints of variable Xij--------------------------//

		//------------------------------------------------end of contriants-------------------------------------//

		//begin of solving 
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, fabs(3600.000000000000));
		clock_t begin = clock();
		cplex.solve();
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}

		int objectiveValue = cplex.getObjValue();
		clock_t end = clock();
		timed = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "bx下界求解时间：" << timed << endl;
		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();
		//model.end();
		cplex.end();
		clock_t end1 = clock();
		timedd = (double)(end1 - begin) / CLOCKS_PER_SEC;
		cout << "timedd" << timedd << endl;
		return objectiveValue;


	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
		abort();
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
		abort();
	}

	env.end();
}

// Operating logarithm of value in base 2
// PRE: 
// PARAM: value - the number to operation logarithm
// POST: Ansewer of logarithm is returned
//int log2(double value)
//{
//	return int(log(value) / log(2.0));
//}
bool func(int ar[], int size) {

	for (int i = 0; i<size; i++) {
		bool b = false;
		for (int j = 0; j<size; j++) {
			if (i == ar[j]) b = true;
		}
		if (!b) return false;
	}
	return true;
}
// Solving the MAP problem
// PRE: 
// PARAM: c - two dimensional array, n - # of rows, n - # of columns, count - number of test
// POST: MAP is linearized by MILP4 formulation and objective value and solution is derived
void solve(int** a, int** b, int** c, int n, int count)
{
	current = new int[n];
	IloEnv env;
	try {
		IloModel model(env);
		clock_t begin = clock();
		int l = minimizeB(b, n);
		int u = maximizeB(b, n);
		int lKnot = minimizeA(a, n);
		int uKnot = maximizeA(a, n);
		clock_t end2 = clock();
		double timex = (double)(end2 - begin) / CLOCKS_PER_SEC;
		int alpha = (int)log2(double(u - l)) + 1;
		int beta= (int)log2(double(uKnot - lKnot)) + 1;
		cout << "alpha=" << alpha << endl;
		cout << "l=" << l << " u=" << u << " lKnot=" << lKnot << " uKnot=" << uKnot << endl;
		/*begin of instantiating objective variables where
		variables are presented in two dimensional array
		and each variable such Xij is {0,1}*/
		IloArray<IloBoolVarArray> x(env, n);
		for (int i = 0; i < n; i++)
		{
			x[i] = IloBoolVarArray(env, n);
		}
	
		char  bufi[16];
		char  bufj[16];
		for (int i = 0; i < n; i++)
		{
			//_itoa_s(i, bufi, sizeof(bufi), 10);
			for (int j = 0; j < n; j++)
			{
				x[i][j] = IloBoolVar(env);
				_itoa_s(i, bufi, sizeof(bufi), 10);//把int i转换成字符i
				_itoa_s(j, bufj, sizeof(bufj), 10);//把int j转换成字符j
				char  xij[16] = "x";
				strcat_s(bufi,16,"_");//"i_"
				strcat_s(bufi,16,bufj);//"i_j"
				strcat_s(xij,16,bufi);//"xi_j"
				x[i][j].setName(xij);
	
			}
			
		}//end of Xij

		IloArray<IloBoolVarArray>  w(env,alpha);
		for (int k = 0; k < alpha; k++)
		{
			w[k] = IloBoolVarArray(env, beta);
		}
		char  bufii[16];
		char  bufjj[16];
		for (int i = 0; i < alpha; i++)
		{
			for (int j = 0; j < beta; j++)
			{
				w[i][j] = IloBoolVar(env);
				_itoa_s(i, bufii, 16, 10);
				_itoa_s(j, bufjj, 16, 10);
				char  wkl[16] = "w";
				strcat_s(bufii, 16, "_");
				strcat_s(bufii, 16, bufjj);
				strcat_s(wkl, 16, bufii);
				w[i][j].setName(wkl);
			}
		}//end of wkl
		IloBoolVarArray v(env);
		char  bufk[16];
		for (int k = 0; k < alpha; k++)
		{
			v.add(IloBoolVar(env));
			_itoa_s(k, bufk, 16, 10);
			char  kk[16] = "k";
			strcat_s(kk, 16, bufk);
			v[k].setName(kk);
		}//end of V
		IloBoolVarArray ul(env);
		char  bufu[16];
		for (int k = 0; k < beta; k++)
		{
			ul.add(IloBoolVar(env));
			_itoa_s(k, bufu, 16, 10);
			char  uu[16] = "u";
			strcat_s(uu, 16, bufu);
			ul[k].setName(uu);
		}//end of ul
		IloNumVar z(env);
		//-----------------------end of instantiating objective variables-------------------------------//

		//--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum1(env);//alpha==tb,beta==ta
		for (int i = 0; i < alpha; i++)
		{
			for (int j = 0; j < beta; j++)
			{
				linear_sum1 += int(pow(2.0, i + j)) * w[i][j];
			}
		}
		IloNumExpr linear_sum2(env);
		for (int i = 0; i < alpha; i++)
		{
				linear_sum2 += int(pow(2.0, i)) * v[i];
		}
		linear_sum2 = linear_sum2*lKnot;
		IloNumExpr linear_sum3(env);
		for (int i = 0; i < beta; i++)
		{
			linear_sum3 += int(pow(2.0, i)) * ul[i];
		}
		linear_sum3 = linear_sum3*l;
		IloNumExpr linear_sum4(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum4 += c[i][j] * x[i][j];
			}
		}

		model.add(IloMinimize(env, l * lKnot + linear_sum1 + linear_sum2 + linear_sum3 + linear_sum4));
		linear_sum1.end();
		linear_sum2.end();
		linear_sum3.end();
		linear_sum4.end();
		//------------------------------end of objective function----------------------------------------//

		/*begin of instantiating contraints where constraint1
		is the sum of x[i][j] from j = 1 to n that is equal to 1,
		i = 1 to n and constraint2 is the sum of of x[i][j]
		from i = 1 to n that is equal to 1;*/
		for (int i = 0; i < n; i++)
		{
			IloNumExpr constraint1(env);
			for (int j = 0; j < n; j++)
			{
				constraint1 += x[i][j];
			}
			model.add(constraint1 == 1);
			constraint1.end();
		}

		for (int j = 0; j < n; j++)
		{
			IloNumExpr constraint2(env);
			for (int i = 0; i < n; i++)
			{
				constraint2 += x[i][j];
			}
			model.add(constraint2 == 1);
			constraint2.end();
		}
		//-------------------------------------------end of constraints of variable Xij--------------------------//
		/*begin of instantiating constraint3 sum of b[i][j] * x[i][j] where j = 1 to n; i = 1 to n ==  constraint4
		l + sum of 2 ^ (k-1) * v[k] where k = 1 to alpha such that constraint 3 = constraint 4*/
		IloNumExpr constraint3(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				constraint3 += b[i][j] * x[i][j];
			}
		}

		IloNumExpr constraint4(env);
		for (int k = 0; k < alpha; k++)
		{
			int kk = int(pow(2.0, k));
			constraint4 += kk * v[k];
			//cout << "int(pow(2.0, k))=" << kk <<" k="<<k<< endl;
		}

		model.add(constraint3 == l + constraint4);
		constraint3.end();
		constraint4.end();
		/*begin of instantiating constraint5 sum of a[i][j] * x[i][j] where j = 1 to n; i = 1 to n ==  constraint6
		lKnot + sum of 2 ^ (k-1) * ul[k] where k = 1 to beta such that constraint 5 = constraint 6*/
		IloNumExpr constraint5(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				constraint5 += a[i][j] * x[i][j];
			}
		}

		IloNumExpr constraint6(env);
		for (int k = 0; k < beta; k++)
		{
			int kk = int(pow(2.0, k));
			constraint6 += kk * ul[k];
			//cout << "int(pow(2.0, k))=" << kk <<" k="<<k<< endl;
		}

		model.add(constraint5 == lKnot + constraint6);
		constraint5.end();
		constraint6.end();
		/*being of instantiating constraint7 w[i][j]-v[i]<=0 where j = 1 to beta and i = 1 to alpha;
		constraint8 w[k] -ul[j]>=0 where j = 1 to beta and i = 1 to alpha; constraint9 w[k] - v[i]-ul[j]+1 >= 0; */
		for (int i = 0; i < alpha; i++)
		{
			for (int j = 0; j < beta; j++)
			{
				model.add(w[i][j] - v[i] <= 0);
				model.add(w[i][j] - ul[j] <= 0);
				model.add(w[i][j] - v[i]-ul[j]+1 >= 0);
			}
		}
		//------------------------------------------------end of contriants-------------------------------------//

		//begin of solving 
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, 3600);
		cplex.setParam(IloCplex::Param::MIP::Limits::AuxRootThreads, -1);//0 is default; -1 if off  
		cplex.setParam(IloCplex::Param::Preprocessing::Presolve, 0);//0 is not presolve ,1 is default presolve
		cplex.setParam(IloCplex::Param::Barrier::Display,2); //IloCplex::Param::Conflict::Display
		cplex.setParam(IloCplex::Param::Conflict::Display,2);
		cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 1e-8);
		cplex.setParam(IloCplex::Param::MIP::Display, 3);//输出在第几个节点找到的整数解和时间
		cplex.setParam(IloCplex::Param::MIP::Interval, 0);//default，间隔多少个节点写入一行
		cplex.setParam(IloCplex::Param::Sifting::Display, 2);//输出详细的错误信息
		cplex.setParam(IloCplex::Param::Output::WriteLevel, 3);//只输出非零离散型变量值
		/*cplex.setParam(IloCplex::Param::Output::IntSolFilePrefix,sol);
		cplex.setParam(IloCplex::Param::Output::CloneLog, 1);*/
		std::ofstream LogFile(logfile);
		cplex.setOut(LogFile);
		clock_t begin1 = clock();
		cplex.solve();
		clock_t end = clock();

		double execution_time = double(end - begin) / CLOCKS_PER_SEC;
		double timey = double(end - begin1) / CLOCKS_PER_SEC;
		string optimizedResult;
		string  opt;
		if (cplex.getStatus() == IloAlgorithm::Optimal && isOptimal)
		{
			optimizedResult = "Solution is optimal";
			opt = "y";
		}
		else if (cplex.getStatus() == IloAlgorithm::Feasible)
		{

			optimizedResult = "Solution is not optimal but feasible";
			opt = "ny";
		}
		else
		{
			optimizedResult = "Solution is not optimal and not feasible";
			opt = "nn";
		}
		long ax = 0, bx = 0, cx = 0;
		long long obj = 0;
		IloNumArray solution(env);
		for (int i = 0; i < n; i++)
		{
			cplex.getValues(solution, x[i]);
			for (int j = 0; j < n; j++)
			{
				if (fabs(solution[j] - 1) < 0.000001)
				{
					current[i] = j;
					cout << current[i] << " ";

					ax += a[i][j] ;
					bx += b[i][j] ;
					cx += c[i][j] ;
				}
			}

		}
		bool xx = func(current, n);
		obj = (long long)ax*bx + cx;
		cout << endl;
		cout << "A(X): " << ax << endl;
		cout << "B(X): " << bx << endl;
		cout << "C(X): " << cx << endl;
		cout << "obj=" << obj << endl;
		long long exactresult = (long long)IloRound(cplex.getObjValue());
		double exactresult2 = cplex.getObjValue();
		cout << exactresult2 << endl;
		//begin of diplaying result
		cout << left << setw(nameWidth) << setfill(separator) << count + 1;
		cout << left << setw(nameWidth) << setfill(separator) << n;
		cout << left << setw(nameWidth) << setfill(separator) << n;
		cout << fixed;
		cout << left << setw(20) << setfill(separator) << exactresult;
		cout << left << setw(13) << setfill(separator) << execution_time;
		cout << left << setw(nameWidth) << setfill(separator) << optimizedResult;
		cout << endl;

		if (execution_time < minimum_execution_time)
		{
			minimum_execution_time = execution_time;
		}

		if (execution_time > maximum_execution_time)
		{
			maximum_execution_time = execution_time;
		}

		total_execution_time += execution_time;
		FILE *fp;
		errno_t err;
		err = fopen_s(&fp, resultfile, "a+");
		fprintf(fp, "best_sol:");
		for (int i = 0; i < n; i++)
			fprintf(fp, "%d ", current[i]);
		fprintf(fp, "\n");
		fprintf(fp, "exact_result:%lld exactresult2:%f is permutation?:%d ", exactresult, exactresult2, xx);
		fprintf(fp, "ax=%ld  bx=%ld  cx=%ld  obj=%lld\n ", ax, bx, cx, obj);
		fprintf(fp, "ax_Upper_bound_time：%f \n ", timeaa);
		fprintf(fp, "ax_lower_bound_time：%f \n ", timebb);
		fprintf(fp, "bx_Upper_bound_time：%f \n ", timecc);
		fprintf(fp, "bx_lower_bound_time：%f \n ", timedd);
		fprintf(fp, "milp3_total_time：%f  ", execution_time);
		fprintf(fp, "milp3_solve_time：%f total_time_of_A(X)B(X)'s lower and upper：%f ", timey, timex);
		fprintf(fp, "is optimal:%s", opt);
		fprintf(fp, "\n***********\n");
		fclose(fp);
		cplex.end();
		//end of solving

	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
		abort();
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
		abort();
	}

	env.end();

}