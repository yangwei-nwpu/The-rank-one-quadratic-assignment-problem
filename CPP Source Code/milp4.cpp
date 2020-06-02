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
//char * sol = "C:/Users/wei yang/Desktop/test/test50/QR1-R-NCRow-L/milp4result/solutionfileQR1-2000-R-NCRow-L";
int main()
{
	//-----------------begining the test of 5 different problems by 5 different problem sizes----------------------------------//
	int count = 0;

	cout << "Followings are the result for MILP4" << endl;
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
	//system("pause");
	return 0;
}
void matrix_transform(int ** a, int ** b, int ** c, int n)
{
	int *amin = new int[n];
	int *amax = new int[n];
	int *ri = new int[n];
	for (int i = 0; i < n; i++)
	{
		amin[i] = 100;
		amax[i] = 0;
		for (int j = 0; j < n; j++)
		{
			if (a[i][j] < amin[i])
				amin[i] = a[i][j];
			if (a[i][j] > amax[i])
				amax[i] = a[i][j];
		}
		if (amax[i] == amin[i])
			ri[i] = 1;
		else
			ri[i] = amax[i] - amin[i] + 1;
	}
	int alp = 0, beta = 0;
	for (int i = 0; i < n; i++)
	{
		alp += amin[i];
		//beta += bp[i];
	}
	
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
		cplex.setParam(IloCplex::AuxRootThreads, -1);
		cplex.setParam(IloCplex::PreInd, 0);
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
	int *amin = new int[n];
	int *amax = new int[n];
	int *ri = new int[n];
	int *alpha = new int[n];
	for (int i = 0; i < n; i++)
	{
		amin[i] = 100;
		amax[i] = 0;
		for (int j = 0; j < n; j++)
		{
			if (a[i][j] < amin[i])
				amin[i] = a[i][j];
			if (a[i][j] > amax[i])
				amax[i] = a[i][j];
		}
		ri[i] = amax[i] - amin[i] ;
		//cout << ri[i]<<" ";
	}
	cout << endl;
	int alp = 0, beta = 0;
	for (int i = 0; i < n; i++)
	{
		alp += amin[i];
		//beta += bp[i];
	}
	IloEnv env;
	try {
		IloModel model(env);
		clock_t begin = clock();
		int l = minimizeB(b, n);
		int u = maximizeB(b, n);
		/*int lKnot = minimizeA(a, n);
		int uKnot = maximizeA(a, n);*/
		clock_t end2 = clock();
		double timex = (double)(end2 - begin) / CLOCKS_PER_SEC;
		for (int i = 0; i < n; i++) {
			if (ri[i] == 0)
				alpha[i] = 1;
			else
				alpha[i] = (int)log2(ri[i])+ 1;
			//cout << alpha[i] << " ";
		}
		cout << endl;
		//int beta = (int)log2(double(uKnot - lKnot)) + 1;
		//cout << "alpha=" << alpha << endl;
		cout << "l=" << l << " u=" << u << endl;
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
				strcat_s(bufi, 16, "_");//"i_"
				strcat_s(bufi, 16, bufj);//"i_j"
				strcat_s(xij, 16, bufi);//"xi_j"
				x[i][j].setName(xij);

			}
		}//end of Xij

		IloArray<IloNumVarArray>  w(env, n);
		for (int i = 0; i < n; i++)
		{
			w[i] = IloNumVarArray(env, alpha[i]);
		}
		char  bufii[16];
		char  bufjj[16];
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < alpha[i]; j++)
			{
				w[i][j] = IloNumVar(env);
				_itoa_s(i, bufii, 16, 10);
				_itoa_s(j, bufjj, 16, 10);
				char  wkl[16] = "w";
				strcat_s(bufii, 16, "_");
				strcat_s(bufii, 16, bufjj);
				strcat_s(wkl, 16, bufii);
				w[i][j].setName(wkl);
			}
		}//end of wik
		IloArray<IloBoolVarArray>  v(env, n);
		for (int i = 0; i < n; i++)
		{
			v[i] = IloBoolVarArray(env, alpha[i]);
		}
		/*char  bufii[16];
		char  bufjj[16];*/
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < alpha[i]; j++)
			{
				v[i][j] = IloBoolVar(env);
				_itoa_s(i, bufii, 16, 10);
				_itoa_s(j, bufjj, 16, 10);
				char  wkl[16] = "v";
				strcat_s(bufii, 16, "_");
				strcat_s(bufii, 16, bufjj);
				strcat_s(wkl, 16, bufii);
				v[i][j].setName(wkl);
			}
		}//end of wik
		IloNumVar y(env);
		//-----------------------end of instantiating objective variables-------------------------------//

		//--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum1(env);//alpha==tb,beta==ta
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < alpha[i]; j++)
			{
				linear_sum1 += int(pow(2.0, j)) * w[i][j];
			}
		}
		
		IloNumExpr linear_sum2(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum2 += c[i][j] * x[i][j];
			}
		}

		model.add(IloMinimize(env, alp*y + linear_sum1 + linear_sum2));
		linear_sum1.end();
		linear_sum2.end();
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
		
		IloNumExpr constraint3(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				constraint3 += b[i][j] * x[i][j];
			}
		}
		model.add(y == constraint3);
		constraint3.end();
		//IloNumExpr constraint4(env);
		for (int i = 0; i < n; i++) 
		{
			IloNumExpr constraint4(env);
			IloNumExpr constraint5(env);
			for (int j = 0; j < n; j++)
			{
				constraint4 += a[i][j] * x[i][j];
			}
			for (int k = 0; k < alpha[i]; k++)
			{
				int kk = int(pow(2.0, k));
				constraint5 += kk*v[i][k];
			}
			model.add(constraint4 == amin[i] + constraint5);
			constraint4.end();
			constraint5.end();
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < alpha[i]; j++)
			{
				model.add(w[i][j] - u*v[i][j] <= 0);
				model.add(w[i][j] - l*v[i][j] >= 0);
				model.add(y - w[i][j] + u*v[i][j] <= u);
				model.add(y - w[i][j] + l*v[i][j] >= l);
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
		cplex.setParam(IloCplex::Param::Barrier::Display, 2); //IloCplex::Param::Conflict::Display
		cplex.setParam(IloCplex::Param::Conflict::Display, 2);
		cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 1e-8);
		cplex.setParam(IloCplex::Param::MIP::Display, 3);//输出在第几个节点找到的整数解和时间
		cplex.setParam(IloCplex::Param::MIP::Interval, 0);//default，间隔多少个节点写入一行
		cplex.setParam(IloCplex::Param::Sifting::Display, 2);//输出详细的错误信息
		cplex.setParam(IloCplex::Param::Output::WriteLevel, 3);//只输出非零离散型变量值
		/*cplex.setParam(IloCplex::Param::Output::IntSolFilePrefix, sol);
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
					//cout << current[i] << " ";

					ax += a[i][j];
					bx += b[i][j];
					cx += c[i][j];
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
		fprintf(fp, "milp4_total_time：%f  ", execution_time);
		fprintf(fp, "milp4_solve_time：%f total_time_of_A(X)B(X)'s lower and upper：%f ", timey, timex);
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
	system("pause");
	env.end();

}