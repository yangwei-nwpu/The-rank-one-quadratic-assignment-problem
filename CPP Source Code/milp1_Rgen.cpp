
#include <ilcplex/ilocplex.h>//cplex
#include <iostream>//std
#include <stdlib.h>//srand,rand
#include <time.h>//clock_t
#include <iomanip>// table display

using namespace std;

//fucntion prototype
void optimization_of_quadratic_transpotation(int);//begining of the the program
int maximizeB(int**, int);//get the value of U by maximize B(X);
int minimizeB(int**, int);//get the value of L by minimize B(X);
void solve(int**, int**, int**, int, int);//solving the quadratic transportation problem
										  //end of function prototype
										  //------------------------------variable declaration----------------------------------//
int n = 10;
int seed = 0;
double minimum_execution_time = DBL_MAX;
double maximum_execution_time = DBL_MIN;
double total_execution_time = 0.0;
bool isOptimal = true;
const char separator = ' ';
const int nameWidth = 8;
long long alphabeta = 0;
//------------------------------end of variable declaration--------------------------//
char * textfilename = "C:/Users/wei yang/Desktop/QAP/test2/QR1-200-R-S.txt";
char * resultfile = "C:/Users/wei yang/Desktop/QAP/ceshicode/QR1-200-R-S.txt";
char * logfile = "C:/Users/wei yang/Desktop/QAP/ceshicode/logQR1-200-R-S.txt";
int main()
{
	//-----------------begining the test of 5 different problems by 5 different problem sizes----------------------------------//
	int count = 0;
	cout << "Followings are the result for MILP1" << endl;
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
	cout << "Average Execution Time: " << total_execution_time / 5 << endl;
	cout << endl;
	cout << endl;
	FILE *fp;
	errno_t err;
	err = fopen_s(&fp, resultfile, "a+");
	fprintf(fp, "Minimum Execution Time:%f\n", minimum_execution_time);
	fprintf(fp, "Maximum Execution Time:%f\n", maximum_execution_time);
	fprintf(fp, "Average Execution Time:%f\n", total_execution_time / 1);
	fclose(fp);
	//system("pause");
	return 0;
}

void matrix_transform(int ** a, int ** b, int ** c, int n)
{
	int *ap = new int[n];
	int *bp = new int[n];
	for (int i = 0; i < n; i++)
	{
		ap[i] = 0;
		bp[i] = 0;
		for (int j = 0; j < n; j++)
		{
			if (a[i][j] < ap[i])
				ap[i] = a[i][j];
			if (b[i][j] < bp[i])
				bp[i] = b[i][j];
		}
	}
	int alp = 0, beta = 0;
	for (int i = 0; i < n; i++)
	{
		alp += ap[i];
		beta += bp[i];
	}
	alp = -alp;
	beta = -beta;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			c[i][j] = c[i][j] - alp*b[i][j] - beta*a[i][j];
			a[i][j] = a[i][j] - ap[i];
			b[i][j] = b[i][j] - bp[i];
		}
	alphabeta = alp*beta;
}
void optimization_of_quadratic_transpotation(int count)
{
	//srand(seed++);
	//int lower_bound, upper_bound;
	int** a;//n x n
	int** b;//n x n
	int** c;//n x n
	long object;
	ifstream ReadFile;
	string temp;
	//char * infilename = "C:/Users/ASUS/Desktop/yangwei/text5.txt";

	ReadFile.open(textfilename, ios::in);//ios::in 表示以只读的方式读取文
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
	matrix_transform(a, b, c, n);
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
		//clock_t begin = clock();
		cplex.solve();
		//clock_t end = clock();
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}
		int objectiveValue = cplex.getObjValue();

		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();
		cplex.end();

		return objectiveValue;

	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
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
		cplex.setParam(IloCplex::AuxRootThreads, -1);
		cplex.setParam(IloCplex::PreInd, 0);
		cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 1e-8);
		cplex.setParam(IloCplex::TiLim, fabs(3600.000000000000));
		//clock_t begin = clock();
		cplex.solve();
		//clock_t end = clock();
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}

		int objectiveValue = cplex.getObjValue();

		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();
		cplex.end();

		return objectiveValue;

	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
	}

	env.end();
}

// Solving the qudratic transportation problem
// PRE: 
// PARAM: a,b,c - two dimensional array, n - # of rows, n - # of columns
// POST:  objective cost is returned
void solve(int** a, int** b, int** c, int n, int count)
{
	int* current;
	current = new int[n];
	clock_t time_begin = clock();
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

		 /*begin of instantiating objective variables where
		 variables are presented in two dimensional array
		 and each variable such Zij is non-binary*/
		IloArray<IloNumVarArray> z(env, n);
		for (int i = 0; i < n; i++)
		{
			z[i] = IloNumVarArray(env, n);
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				z[i][j] = IloNumVar(env);
			}
		}

		IloNumVar y(env);
		//-----------------------end of instantiating objective variables-------------------------------//


		//--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum1(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum1 += a[i][j] * z[i][j];
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

		model.add(IloMinimize(env, linear_sum1 + linear_sum2));
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
		//-------------------------------------------end of constraints of variable Xij--------------------------//
		/*begin of instantiating constraint3 y = b[i][j]x[i][j] for all i and j
		constraint4 is z[i][j] - u * x[i][j] <= 0 for all i,j, constraint5 is
		z[i][j] - l * x[i][j] >= 0 for all i,j, constraint6 is y - z[i][j] + u * x[i][j] <= u
		,constraint7 is y - z[i][j] + u * x[i][j] >= u, constraint8 is l <= y and finally
		constraint8 is y <= u*/
		int u = maximizeB(b, n);
		int l = minimizeB(b, n);
		cout << "u=" << u << " l=" << l << endl;
		IloNumExpr constraint3(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				constraint3 += b[i][j] * x[i][j];
			}
		}
		model.add(constraint3 == y);

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				model.add(z[i][j] - u * x[i][j] <= 0);
				model.add(z[i][j] - l * x[i][j] >= 0);
				model.add(y - z[i][j] + u * x[i][j] <= u);
				model.add(y - z[i][j] + l * x[i][j] >= l);
			}
		}

		constraint3.end();
		//------------------------------------------------end of contriants-------------------------------------//

		//begin of solving 
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, fabs(3600.000000000000));
		cplex.setParam(IloCplex::AuxRootThreads, -1);
		cplex.setParam(IloCplex::PreInd, 0);
		std::ofstream LogFile(logfile);
		cplex.setOut(LogFile);
		clock_t begin = clock();
		cplex.solve();
		clock_t end = clock();
		long long objvalue = cplex.getObjValue() - alphabeta;
		double execution_time = double(end - time_begin) / CLOCKS_PER_SEC;
		cout << time_begin << " " << begin << endl;
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
		IloNumArray solution(env);
		for (int i = 0; i < n; i++)
		{
			cplex.getValues(solution, x[i]);
			for (int j = 0; j < n; j++)
			{
				if (fabs(solution[j] - 1) <0.000001)
				{
					current[i] = j;
					cout << current[i] << " ";
				}
				ax += a[i][j] * solution[j];
				bx += b[i][j] * solution[j];
				cx += c[i][j] * solution[j];

			}
		}

		cout << endl;
		cout << "A(X): " << ax << endl;
		cout << "B(X): " << bx << endl;
		cout << "C(X): " << cx << endl;
		//begin of diplaying result
		cout << left << setw(nameWidth) << setfill(separator) << count + 1;
		cout << left << setw(nameWidth) << setfill(separator) << n;
		cout << left << setw(nameWidth) << setfill(separator) << n;
		cout << fixed;
		cout << left << setw(20) << setfill(separator) << cplex.getObjValue();
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
		fprintf(fp, "exact_result:%lld  ", objvalue);
		fprintf(fp, "ax=%ld  bx=%ld  cx=%ld\n", ax, bx, cx);
		fprintf(fp, "milp1_total_time：%f  ", execution_time);
		fprintf(fp, "is optimal:%s", opt);
		fprintf(fp, "\n***********\n");
		fclose(fp);
		//de-allocate heap memory
		for (int i = 0; i < n; i++)
		{
			x[i].end();
			z[i].end();
		}
		x.end();
		z.end();
		y.end();
		cplex.end();
	}
	catch (IloException& e)
	{
		cout << endl;
		cerr << "concert exception caught: " << e << endl;
	}
	catch (...)
	{
		cout << endl;
		cerr << "Unknown exception caught" << endl;
	}

	env.end();
}