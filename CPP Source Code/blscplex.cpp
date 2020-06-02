
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <map>
#include<ctime>
#include<cmath>

typedef int*   type_vector;
typedef long long** type_matrix;

const long long infinite = 9999999999;

using namespace std;

long iteration = 0;

//BLS parameters
double r1 = 0.7, r2 = 0.2; // Parameters used for directed perturbation. Number of moves elapsed before perturb.
double init_pert_str = 0.15; //initial perturbation strength
double perturb_str; // actual perturbation strength
int T = 2000;
double P0 = 0.75, Q = 0.3;
long num_iterations = 10000;
double cplextime = 0.0;
int max_restart = 1000;
char * parameterfile = "C:/Users/wei yang/Desktop/QAP/test/parameter.txt";
char * textfilename = "C:/Users/wei yang/Desktop/QAP/test2/QR1-2000-R-S.txt";
char * resultfilename = "C:/Users/wei yang/Desktop/QAP/BLS/QR1-2000-R-s.txt";
bool isOptimal = true;
clock_t cplexbegin, cplexend;
int **startsol = new int*[4];
int *tempsola;
int *tempsolb;
int ** D;
clock_t time1, time2, time3, time4, totaltime1, totaltime2;
void transpose(int & a, int & b) { long temp = a; a = b; b = temp; }


/*--------------------------------------------------------------*/
/*        update_matrix of the a_p,b_p,c_p,value after swap(i,j)      */
/*-------------------------------------------------------------*/

void update_matrix_after_change(int i_retained, int j_retained, long n, type_matrix & value, type_vector & p, type_matrix & a, type_matrix & b, type_matrix & c,
	type_matrix & a_p, type_matrix & b_p, type_matrix & c_p)
{
	int i, j, current_a_p, current_b_p, current_c_p;
	current_a_p = a_p[i_retained][j_retained];
	current_b_p = b_p[i_retained][j_retained];
	current_c_p = c_p[i_retained][j_retained];
	for (i = 1; i < n; i = i + 1)
		for (j = i + 1; j <= n; j = j + 1)
		{
			a_p[i][j] = current_a_p + a[i][p[j]] + a[j][p[i]] - a[i][p[i]] - a[j][p[j]];
			b_p[i][j] = current_b_p + b[i][p[j]] + b[j][p[i]] - b[i][p[i]] - b[j][p[j]];
			c_p[i][j] = current_c_p + c[i][p[j]] + c[j][p[i]] - c[i][p[i]] - c[j][p[j]];
			value[i][j] = a_p[i][j] * b_p[i][j] + c_p[i][j];
		}

}

void apply_move(type_vector & p, long n, type_matrix & value, long long& current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long& best_value, type_vector & best_sol,
	int i_retained, int j_retained, clock_t & end_time)
{
	if (i_retained != -1 && j_retained != -1) // apply the selected perturbation move
	{
		last_swaped[i_retained][j_retained] = iteration;
		last_swaped[j_retained][i_retained] = iteration;
		swap(p[i_retained], p[j_retained]);
		current_value = value[i_retained][j_retained];
		update_matrix_after_change(i_retained, j_retained, n, value, p, a, b, c, a_p, b_p, c_p);
		if (current_value<best_value)
		{
			best_value = current_value;
			end_time = clock();
			iter_without_improvement = 0;
			for (int m = 1; m <= n; m = m + 1) best_sol[m] = p[m];
			//cout << "Solution of value " << best_value<< " found. " <<iteration<< endl;

		}
	} iteration++;
}
void tabu_search_perturb(type_vector & p, long n, type_matrix & value, long long& current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long& best_value, type_vector & best_sol, long long init_value,
	clock_t & end_time)
{
	int i_retained = -1, j_retained = -1, i, j;
	long long min_value;
	min_value = infinite;
	min_value = 9999999999;
	for (i = 1; i < n; i = i + 1)
	{
		for (j = i + 1; j <= n; j = j + 1)
		{
			if (value[i][j] != init_value && value[i][j] < min_value && ((last_swaped[i][j] + n*r1 + rand() % (static_cast<int>(n*r2)))<iteration || (value[i][j]<best_value)))
			{
				i_retained = i; j_retained = j;
				min_value = value[i][j];
			};
		};
	};
	apply_move(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, i_retained, j_retained, end_time);
}
void recency_based_perturb(type_vector & p, long n, type_matrix & value, long long & current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long & best_value, type_vector & best_sol, clock_t & end_time)
{
	int i, j, i_retained, j_retained;
	long long min = infinite;

	for (i = 1; i < n; i = i + 1)
	{
		for (j = i + 1; j <= n; j = j + 1)
		{
			if (last_swaped[i][j]<min)
			{
				i_retained = i; j_retained = j;
				min = last_swaped[i][j];
			};
		};
	};
	apply_move(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, i_retained, j_retained, end_time);
}
void random_perturb(type_vector & p, long n, type_matrix & value, long long & current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long & best_value, type_vector & best_sol, long long init_value, clock_t & end_time)
{
	int i_retained = 1 + rand() % n;
	int j_retained = 1 + rand() % n;
	if (i_retained>j_retained)
		swap(i_retained, j_retained);
	while (i_retained == j_retained || (a_p[i_retained][j_retained] * b_p[i_retained][j_retained] + c_p[i_retained][j_retained]) == init_value)
	{
		j_retained = 1 + rand() % n;
		if (i_retained>j_retained)
			swap(i_retained, j_retained);
	}
	apply_move(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, i_retained, j_retained, end_time);
}
void perturbe(type_vector & p, long n, type_matrix & value, long long& current_value, type_matrix & a, type_matrix & b, type_matrix & c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, type_vector & best_sol, long long  &best_value, clock_t & end_time)
{
	int i_retained = -1, j_retained = -1, k;
	long long cost = current_value;
	double d = static_cast<double>(iter_without_improvement) / T;
	double e = pow(2.718, -d);
	int perturbation_bit = 0;
	if (e<P0)
		e = P0;
	if (e>(static_cast<double>(rand() % 101) / 100.0))
		perturbation_bit = 1;

	for (k = 0; k<(perturb_str); k++)
	{
		i_retained = -1; j_retained = -1;
		if (perturbation_bit == 1)
		{
			tabu_search_perturb(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, cost, end_time);
		}
		else
		{
			if (Q>(static_cast<double>(rand() % 101) / 100.0))
				recency_based_perturb(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, end_time);
			else
				random_perturb(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, cost, end_time);

		}

	}
}
long long value_compute(long n,type_matrix a, type_matrix b, type_matrix c,type_vector & best_sol)
{
	int x=0,y=0,z=0; 
	for (int i = 1; i <= n; i = i + 1) 
	{
		x=x+a[i][best_sol[i]];
		y=y+b[i][best_sol[i]];
		z=z+c[i][best_sol[i]];
    }
	return (long long)(x*y+z);
}
void allocate_memory_and_initialize(type_vector & p, long n, type_matrix & value, type_matrix & last_swaped, long long& current_value,
	type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p, type_vector & best_sol, long long & best_value)
{
	int i, j;
	/***************** dynamic memory allocation *******************/
	p = new int[n + 1];
	a_p = new long long*[n + 1];
	b_p = new long long*[n + 1];
	c_p = new long long*[n + 1];
	value = new long long*[n + 1];
	last_swaped = new long long*[n + 1];
	for (i = 1; i <= n; i = i + 1)
	{
		a_p[i] = new long long[n + 1];
		b_p[i] = new long long[n + 1];
		c_p[i] = new long long[n + 1];
		value[i] = new long long[n + 1];
		last_swaped[i] = new long long[n + 1];
	}
	/************** current solution initialization ****************/
	for (i = 1; i <= n; i = i + 1)
		p[i] = best_sol[i];
	/************** tabu list initialization ****************/
	for (i = 1; i <= n; i = i + 1)
	{
		for (j = 1; j <= n; j++)
		{
			last_swaped[i][j] = 0;
		}
	}
	/********** initialization of current solution value ***********/
	/**************** and matrix of value of moves  ****************/
	current_value = 0;
	long long x = 0, y = 0, z = 0;
	for (i = 1; i <= n; i = i + 1)
	{
		x = x + a[i][p[i]];
		y = y + b[i][p[i]];
		z = z + c[i][p[i]];
	}
	for (i = 1; i < n; i = i + 1)
		for (j = i + 1; j <= n; j = j + 1)
		{
			a_p[i][j] = x + a[i][p[j]] + a[j][p[i]] - a[i][p[i]] - a[j][p[j]];
			b_p[i][j] = y + b[i][p[j]] + b[j][p[i]] - b[i][p[i]] - b[j][p[j]];
			c_p[i][j] = z + c[i][p[j]] + c[j][p[i]] - c[i][p[i]] - c[j][p[j]];
			value[i][j] = a_p[i][j] * b_p[i][j] + c_p[i][j];
		}
	current_value = x*y + z;
	best_value = current_value;
	cout << current_value <<" ok"<<endl;

}

bool best_improvement_move(type_vector & p, long n, type_matrix & value, long long & current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long & best_value, type_vector & best_sol, clock_t & end_time)
{
	int i, j, i_retained = 0, j_retained = 0;
	long long min_value = infinite;   // retained move value
								 //select the best swap move for the descent local search phase
	for (i = 1; i < n; i = i + 1)
	{
		for (j = i + 1; j <= n; j = j + 1)
		{

			if (value[i][j] < min_value)
			{
				i_retained = i; j_retained = j;
				min_value = value[i][j];
			};
		};
	};
	// apply the above selected move if it leads to a solution better than the current one
	if (min_value<current_value)
	{
		//printf("min_value:%ld\n",min_value);
		apply_move(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, i_retained, j_retained, end_time);
		return 1;
	}
	else
		return 0;
}
void determine_jump_magnitude(int & iter_without_improvement, int descent_num, long long previous_value,
	long long current_value, long n)
{
	//the following lines determine the number of perturbation moves (jump magnitude)
	if (iter_without_improvement>T) // if the best found solution is not improved during the last T descent phases
	{
		// strong pertubation required
		iter_without_improvement = 0;
		perturb_str = n*(0.4 + (static_cast<double>(rand() % 20) / 100.0));
	}
	else if ((descent_num != 0 && previous_value != current_value) || (descent_num != 0 && previous_value != current_value))
		// Escaped from the previous local optimum.
	{
		iter_without_improvement++;
		perturb_str = ceil(n*init_pert_str); if (perturb_str<5) perturb_str = 5;
	}
	else if (previous_value == current_value) // search returned to the previous local optimum
	{
		perturb_str += 1;
	}
}
void display_result(FILE *fp, type_vector &best_p, int n, long long best_value, long long best_objective, clock_t & end_time)
{
	long d = 100.0*static_cast<double>(best_value - best_objective) / static_cast<double>(best_objective);
	fprintf(fp, "best_sol:");
	for (int i = 1; i <= n; i++)
		fprintf(fp, "%d ", best_p[i]);
	fprintf(fp, "\n");
	fprintf(fp, "best_value:%ld ", best_value);
	fprintf(fp, "%f ", (end_time) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "%ld", d);
	fprintf(fp, "time1:%f  ", (time1) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "time2:%f  ", (time2) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "time3:%f  ", (time3) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "time4:%f  ", (time4) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "totaltime1:%f  ", (totaltime1) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "totaltime2:%f  ", (totaltime2) / static_cast<double>(CLOCKS_PER_SEC));
	fprintf(fp, "\n***********\n");

}

void BLS(long n,                  // problem size
	type_matrix  a,         // a matrix
	type_matrix  b,         // b matrix
	type_matrix  c,         // c matrix
	type_vector & best_sol,  // best solution found
	long long & best_value,      // cost of best solution
	long num_iterations, long long best_objective, clock_t & end_time)

{
	type_vector p;                        // current solution
	type_matrix a_p;
	type_matrix b_p;
	type_matrix c_p;
	type_matrix value;
	type_matrix last_swaped; // keeps track of the iteration number when a move was last performed

	long current_iteration;
	long long current_value, previous_value;    // current sol. value and previous sol. value
	int descent_num;
	int iter_without_improvement = 0; // counter of the number of consecutive descent phases with no improvement of the best solution
	perturb_str = ceil(init_pert_str*n); // initialize the number of perturbation moves
	bool descent;

	allocate_memory_and_initialize(p, n, value, last_swaped, current_value, a, b, c, a_p, b_p, c_p, best_sol, best_value);
	previous_value = current_value;
	clock_t start = clock();
	end_time = start;
	/******************** main BLS loop ********************/
	for (current_iteration = 1; current_iteration <= num_iterations; current_iteration++)
	{
		//best improvement descent procedure
		descent = best_improvement_move(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, end_time);
		descent_num = 0;
		while (descent) // while an improving solution in the neighborhood exists
		{
			descent = best_improvement_move(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_value, best_sol, end_time);
			descent_num++;
		}

		// a this point, a local optimum is attained and BLS enters a perturbation phase
		determine_jump_magnitude(iter_without_improvement, descent_num, previous_value, current_value, n);

		//update the objective value of the previous local optimum
		previous_value = current_value;
		//cout <<previous_value<<" "<<current_iteration<<endl;
		perturbe(p, n, value, current_value, a, b, c, a_p, b_p, c_p, last_swaped, iter_without_improvement, best_sol, best_value, end_time);
		if (ceil((clock() - start) / static_cast<double>(CLOCKS_PER_SEC)) >= 1800 || (best_value<best_objective))
			break;
	}
	cout << end_time << " " << start << endl;
	end_time =end_time -  start;
	long long best_coputevalue;
	best_coputevalue=value_compute(n,a,b,c,best_sol);
	cout <<"best_sol:"<<endl;
	for(int i=1;i<=n;i++)
	cout <<best_sol[i]<<" ";
	cout<<endl;
	cout<<"best_coputevalue:"<<best_coputevalue<<endl;
	//char * outfilename = "C:/Users/ASUS/Desktop/yangwei/result.txt";

	FILE *fp;
	errno_t err;
	err=fopen_s(&fp,resultfilename, "a+");
	if (err = 0)
	{
		cout << " open file is ok" << endl;
	}
	display_result(fp, best_sol, n, best_value, best_objective, end_time);
	fclose(fp);

	// free memory
	delete[] p;
	for (int i = 1; i <= n; i = i + 1)
	{
		delete[] a_p[i];
		delete[] b_p[i];
		delete[] c_p[i];
		delete[] value[i];
		delete[] last_swaped[i];
	}
	delete[] a_p;
	delete[] b_p;
	delete[] c_p;
	delete[] value;
	delete[] last_swaped;
} // BLS

void generate_random_solution(long n, type_vector  & p)
{
	int i;
	for (i = 0; i <= n; i = i + 1) p[i] = i;
	for (i = 1; i < n; i = i + 1) transpose(p[i], p[i + rand() % (n - i + 1)]);
}
void load_parameter()
{
	ifstream read;
	string temp;
	read.open(parameterfile, ios::in);
	read >> r1;
	read >> r2;
	read >> init_pert_str;
	read >> T;
	read >> P0;
	read >> Q;
	read >> num_iterations;
	read >> max_restart;
	//read>>resultfilename;
	//read>>textfilename;
	read.close();

}
void load_problem(int &n, type_matrix &a, type_matrix &b, type_matrix &c, long long & best_objective)
{   //cout<<"best_objective n"<<endl;
	//cin >> best_objective >> n;
	ifstream ReadFile;
	string temp;
	//char * infilename = "C:/Users/ASUS/Desktop/yangwei/text5.txt";
	ReadFile.open(textfilename, ios::in);//ios::in 表示以只读的方式读取文
	ReadFile >> n;
	ReadFile >> best_objective;
	cout << n << " " << best_objective << endl;
	a = new long long*[n + 1];
	b = new long long*[n + 1];
	c = new long long*[n + 1];
	for (int i = 0; i <= n; i = i + 1)
	{
		a[i] = new long long[n + 1];
		b[i] = new long long[n + 1];
		c[i] = new long long[n + 1];
	}

	for (int i = 1; i <= n; i = i + 1)
	{
		
		for (int j = 1; j <= n; j = j + 1)
		{
			ReadFile >> a[i][j];
			
		}
		
	}
	for (int i = 1; i <= n; i = i + 1)
	{
		
		for (int j = 1; j <= n; j = j + 1)
		{
			ReadFile >> b[i][j];
			
		}
		
	}
	for (int i = 1; i <= n; i = i + 1)
	{
		
		for (int j = 1; j <= n; j = j + 1)
		{
			ReadFile >> c[i][j];
			
		}
		
	}
	ReadFile.close();

}
int minimize(type_matrix& b, int n)
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
				linear_sum += b[i+1][j+1] * x[i][j];
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
int maximize(type_matrix & b, int n)
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
				linear_sum += b[i+1][j+1] * x[i][j];
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
		//clock_t begin = clock();
		cplex.solve();
		//clock_t end = clock();
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}

		int objectiveValue = cplex.getObjValue();
		//cout << "ub=" << objectiveValue << endl;
		//de-allocate heap memory
		/*for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();*/
		return objectiveValue;
		//end of displaying result
		//env.out() << "Cost:" << cplex.getObjValue() << endl;
		//return cplex.getObjValue();
		//end of solving

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
void secondstart(type_matrix & a, type_matrix &b, type_matrix &c, long n, int lnu, type_vector & place)
{
	//long alpha = 0;
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
				//alpha = alpha + b[i + 1][j + 1];
			}
		}
		//alpha = alpha / n;
		//cout << alpha << "ok" << endl;
		//end of Xij
		//-----------------------end of instantiating objective variables-------------------------------//


		//--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum +=( b[i + 1][j + 1] * lnu + a[i + 1][j + 1]  + c[i + 1][j + 1]) * x[i][j];
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
		clock_t begin1 = clock();
		cplex.solve();
		clock_t end1 = clock();
		cplextime = (double)(end1 - begin1);
		//cout << cplextime << endl;
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}
		clock_t end2 = clock();
		//cout << end2 << endl;
		//int objectiveValue = cplex.getObjValue();
		IloNumArray solution(env);
		for (int i = 0; i < n; i++)
		{
			cplex.getValues(solution, x[i]);
			for (int j = 0; j < n; j++)
			{
				//env.out() << "x[" << i << "]" << "[" << j << "] = " << solution[j] << endl;
				if (fabs(solution[j] - 1) < 1e-4)
				{
					place[i + 1] = j + 1;
					//startsol[1][i + 1] = j + 1;

				}
			}
		}
		clock_t end3 = clock();
		//cout << end3 << endl;
		//end of displaying result
		//env.out() << "Cost:" << cplex.getObjValue() << endl;
		//return cplex.getObjValue();
		//end of solving
		//de-allocate heap memory
		/*for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();*/
		
		clock_t end4 = clock();
		//cout << end4 << endl;
		//return objectiveValue;

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
void firststart(type_matrix & a, type_matrix &b, type_matrix &c, long n,int lnu,type_vector & place)
{
	//long alpha = 0;
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
				//alpha = alpha + b[i + 1][j + 1];
			}
		}
		//alpha = alpha / n;
		//cout << alpha << "ok" << endl;
		//end of Xij
		//-----------------------end of instantiating objective variables-------------------------------//


		//--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				D[i + 1][j + 1] = a[i + 1][j + 1] * lnu + b[i + 1][j + 1] + c[i + 1][j + 1];
				linear_sum += D[i+1][j+1]* x[i][j];
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
		clock_t begin1 = clock();
		cplex.solve();
		clock_t end1 = clock();
		cplextime = (double)(end1 - begin1);
		cout << cplextime << endl;
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}
		clock_t end2 = clock();
		//cout << end2 << endl;
		//int objectiveValue = cplex.getObjValue();
		IloNumArray solution(env);
		for (int i = 0; i < n; i++)
		{
			cplex.getValues(solution, x[i]);
			for (int j = 0; j < n; j++)
			{
				//env.out() << "x[" << i << "]" << "[" << j << "] = " << solution[j] << endl;
				if (fabs(solution[j] - 1) < 1e-4)
				{
					place[i + 1] = j + 1;
					//startsol[1][i + 1] = j + 1;

				}
			}
		}
		clock_t end3 = clock();
		//cout << end3 << endl;
		//end of displaying result
		//env.out() << "Cost:" << cplex.getObjValue() << endl;
		//return cplex.getObjValue();
		//end of solving
		//de-allocate heap memory
		/*for (int i = 0; i < n; i++)
		{
			x[i].end();
		}
		x.end();
		model.end();
		cplex.end();*/
		clock_t end4 = clock();
		//cout << end4 << endl;
		//return objectiveValue;

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
void thirdstart(type_matrix & a, type_matrix &b,type_matrix &c, long n)
{   
	long alpha = 0;
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
				alpha = alpha + b[i+1][j+1];
			}
		}
		alpha = alpha / n;
		//cout << alpha<<"ok"<<endl;
		//end of Xij
		 //-----------------------end of instantiating objective variables-------------------------------//


		 //--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum += a[i+1][j+1] * x[i][j]*alpha+c[i+1][j+1]*x[i][j];
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
		clock_t begin1 = clock();
		cplex.solve();
		clock_t end1 = clock();
		cplextime = (double)(end1 - begin1);
		//cout << cplextime << endl;
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}
		clock_t end2 = clock();
		//cout << end2 << endl;
		//int objectiveValue = cplex.getObjValue();
		IloNumArray solution(env);
		for (int i = 0; i < n; i++)
		  {
			cplex.getValues(solution,x[i]);
			for (int j = 0; j < n; j++)
			{
				//env.out() << "x[" << i << "]" << "[" << j << "] = " << solution[j] << endl;
				if (fabs(solution[j]-1) < 1e-4)
				{
					//place[i + 1] = j + 1;
					startsol[2][i + 1] = j + 1;

				}
			}
		  }
		clock_t end3 = clock();
		//cout << end3 << endl;
		//end of displaying result
		//env.out() << "Cost:" << cplex.getObjValue() << endl;
		//return cplex.getObjValue();
		//end of solving
		//de-allocate heap memory
		clock_t end4 = clock();
		//cout << end4 << endl;
		//return objectiveValue;

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
void fourstart(type_matrix & a, type_matrix &b, type_matrix &c, long n)
{
	long alpha = 0;
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
				alpha = alpha + a[i + 1][j + 1];
			}
		}
		alpha = alpha / n;
		//cout << alpha << "ok" << endl;
		//end of Xij
		//-----------------------end of instantiating objective variables-------------------------------//


		//--------------------------begin of instantiating objective function---------------------------//
		IloNumExpr linear_sum(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				linear_sum += b[i + 1][j + 1] * x[i][j] * alpha + c[i + 1][j + 1] * x[i][j];
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
		clock_t begin1 = clock();
		cplex.solve();
		clock_t end1 = clock();
		cplextime = (double)(end1 - begin1);
		//cout << cplextime << endl;
		if (cplex.getStatus() == !IloAlgorithm::Optimal)
		{
			isOptimal = false;
		}
		clock_t end2 = clock();
		//cout << end2 << endl;
		//int objectiveValue = cplex.getObjValue();

		IloNumArray solution(env);
		for (int i = 0; i < n; i++)
		{
			cplex.getValues(solution, x[i]);
			for (int j = 0; j < n; j++)
			{
				if (fabs(solution[j]-1) <1e-4 )
				{
					//place[i + 1] = j + 1;
					startsol[3][i + 1] = j + 1;
				}
			}
		}
		clock_t end3 = clock();
		//cout << end3 << endl;
		//end of displaying result
		//env.out() << "Cost:" << cplex.getObjValue() << endl;
		//return cplex.getObjValue();
		//end of solving
		//de-allocate heap memory
		cplex.end();
		clock_t end4 = clock();
		//cout << end4 << endl;
		//return objectiveValue;

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
int n;                    // problem size
type_matrix a, b, c;         // flows and distances matrices
type_vector solution;     // solution (permutation)
long long cost, best_objective, best_cost = 9999999999;                // solution value
int i, j; //variables for iterations
int main()
{
	long long totalcost=0;
	double totaltime = 0.0;
	
	load_parameter();
	cout << r1 << " " << r2 << endl;
	load_problem(n, a, b, c, best_objective);
	for (int i = 0; i < 4; i++)
	{
		startsol[i] = new int[n+1];
	}
	D = new int*[n+1];
	for (int j = 1; j <= n; j++)
	{
		D[j] = new int[n+1];
	}
	srand(time(NULL));
	/****************** dynamic memory allocation ******************/
	solution = new int[n + 1];
	tempsola = new int[n + 1];
	tempsolb = new int[n + 1];
	long long tempvaluea = 9999999999, tempvalueb = 9999999999;
	long long computevaluea, computevalueb;
	/************** generate random solution and run BLS algorithm **************/
	/*初始化startsol*/
	int lb = minimize(b, n);
	int ub = maximize(b, n);
	int la = minimize(a, n);
	int ua = maximize(a, n);
	int lua = (ua - la) / 9;
	int lub = (ub - lb) / 9;
	for (int i = 0; i < 10; i++)
	{
		clock_t begin = clock();
		firststart(a, b, c, n, lb, tempsola);
		clock_t end1 = clock();
		secondstart(a, b, c, n, la, tempsolb);
		clock_t end2 = clock();
		computevaluea = value_compute(n, a, b, c, tempsola);
		computevalueb = value_compute(n, a, b, c, tempsolb);
		if (computevaluea < tempvaluea)
		{
			tempvaluea = computevaluea;
			time1 = end1 - begin;//跟新最好初始解的时间
			for (int j = 1; j <= n; j++)
			{
				startsol[0][j] = tempsola[j];
			}

		}
		if (computevalueb < tempvalueb)
		{
			tempvalueb = computevalueb;
			time2 = end2 - end1;
			for (int j = 1; j <= n; j++)
			{
				startsol[1][j] = tempsolb[j];
			}

		}
		lb += lub;
		la += lua;
		totaltime1 += end1 - begin;
		totaltime2 += end2 - end1;
		cout << "lb=" << lb << " ,la=" << la << endl;
	}
	clock_t begin2=clock();
	thirdstart(a, b, c, n);
	clock_t end3 = clock();
	fourstart(a, b, c, n);
	clock_t end4 = clock();
	time3 = end3 - begin2;
	time4 = end4 - end3;
	//long num_iterations = 100000;
	/*cout << "startsol:";
	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j <= n; j++)
		{
			cout << startsol[i][j] << " ";
		}
		cout << endl;
	}*/
	clock_t time;
	for (int restart = 0; restart < 4; restart++)
	{
		//generate_random_solution(n, solution);
		
		//cplexbegin = clock();
		//thirdstart(a, b, c, n);
		for (int i = 1; i <= n; i++)
		{
			solution[i] = startsol[restart][i];
			//cout << solution[i] << " ";
		}
		cout << endl;
		iteration = 0;
		BLS(n, a, b, c, solution, cost, num_iterations, best_objective, time);
	
		cout << "Solution value " << cost << " " << (time) / static_cast<double>(CLOCKS_PER_SEC) << " " << 100.0*static_cast<double>(cost - best_objective) / static_cast<double>(best_objective) << endl;
		
		/*totalcost += cost;
		totaltime += time;
*/
	}
	//totalcost = totalcost / 10;

	//cout << totalcost<<" "<<totaltime/10;
	delete[]solution;
	delete[]tempsola;
	delete[]tempsolb;
	for (int i = 0; i < 4; i++)
	{
		delete[]startsol[i];
	}
	delete[]startsol;
	//system("pause");
}
