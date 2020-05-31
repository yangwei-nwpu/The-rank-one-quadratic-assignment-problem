
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <map>
#include<ctime>
#include<cmath>

typedef int*   type_vector;
typedef long long** type_matrix;

const long long infinite = 99999999999;

using namespace std;

long iteration = 0;

//BLS parameters
double r1 = 0.7, r2 = 0.2; // Parameters used for directed perturbation. Number of moves elapsed before perturb.
double init_pert_str = 0.15; //initial perturbation strength
double perturb_str; // actual perturbation strength
int T = 2000;
double P0 = 0.75, Q = 0.3;
long num_iterations = 10000;
int max_restart = 1000;
char * parameterfile = "parameter.txt";
char * resultfilename = "result/QR1-2000-R-S.txt";
char * textfilename = "QR1-R-S/QR1-2000-R-S.txt";

void transpose(int & a, int & b) { long temp = a; a = b; b = temp; }

/*--------------------------------------------------------------*/
/*        update_matrix of the a_p,b_p,c_p,value after swap(i,j)      */
/*-------------------------------------------------------------*/

void update_matrix_after_change(int i_retained, int j_retained, long n, type_matrix & value, type_vector & p, type_matrix & a, type_matrix & b, type_matrix & c,
	type_matrix & a_p, type_matrix & b_p, type_matrix & c_p)
{
	int i, j;
	long current_a_p, current_b_p, current_c_p;
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

void apply_move(type_vector & p, long n, type_matrix & value, long long & current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long & best_value, type_vector & best_sol,
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
void tabu_search_perturb(type_vector & p, long n, type_matrix & value, long long & current_value, type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, long long & best_value, type_vector & best_sol, long long init_value,
	clock_t & end_time)
{
	int i_retained = -1, j_retained = -1, i, j;
	long long min_value;
	min_value = infinite;
	min_value = 99999999999;
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
	long long min = 99999999999;

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
void perturbe(type_vector & p, long n, type_matrix & value, long long & current_value, type_matrix & a, type_matrix & b, type_matrix & c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p,
	type_matrix & last_swaped, int & iter_without_improvement, type_vector & best_sol, long long & best_value, clock_t & end_time)
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
/*long value_compute(long n,type_matrix a, type_matrix b, type_matrix c,type_vector & best_sol)
{
int x=0,y=0,z=0;
for (int i = 1; i <= n; i = i + 1)
{
x=x+a[i][best_sol[i]];
y=y+b[i][best_sol[i]];
z=z+c[i][best_sol[i]];
}
return(x*y+z);
}*/
void allocate_memory_and_initialize(type_vector & p, long n, type_matrix & value, type_matrix & last_swaped, long long & current_value,
	type_matrix a, type_matrix b, type_matrix c, type_matrix & a_p, type_matrix & b_p, type_matrix & c_p, type_vector & best_sol, long long & best_value)
{
	int i, j;
	/***************** dynamic memory allocation *******************/
	p = new int[n + 1];
	a_p = new  long long*[n + 1];
	b_p = new  long long*[n + 1];
	c_p = new  long long*[n + 1];
	value = new  long long*[n + 1];
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
	cout << current_value << endl;

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
	end_time -= start;
	//long best_coputevalue;
	//best_coputevalue=value_compute(n,a,b,c,best_sol);
	//cout <<"best_sol:"<<endl;
	//for(int i=1;i<=n;i++)
	//cout <<best_sol[i]<<" ";
	//cout<<endl;
	//cout<<"best_coputevalue:"<<best_coputevalue<<endl;
	//char * outfilename = "C:/Users/ASUS/Desktop/yangwei/result.txt";
	FILE *fp= fopen(resultfilename, "a+");

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
	for (int i = 1; i <= n; i = i + 1)
	{
		a[i] = new long long[n + 1];
		b[i] = new long long[n + 1];
		c[i] = new long long[n + 1];
	}

	for (int i = 1; i <= n; i = i + 1)
		for (int j = 1; j <= n; j = j + 1)
		{
			ReadFile >> a[i][j];
		}
	for (int i = 1; i <= n; i = i + 1)
		for (int j = 1; j <= n; j = j + 1)
			ReadFile >> b[i][j];
	for (int i = 1; i <= n; i = i + 1)
		for (int j = 1; j <= n; j = j + 1)
			ReadFile >> c[i][j];
	ReadFile.close();

}

int n;                    // problem size
type_matrix a, b, c;         // flows and distances matrices
type_vector solution;     // solution (permutation)
long long cost, best_objective, best_cost = 99999999999;                // solution value
int i, j; //variables for iterations
long totalcost = 0;
double totaltime = 0.0;
int main()
{
	load_parameter();
	cout << r1 << " " << r2 << endl;
	load_problem(n, a, b, c, best_objective);
	srand(time(NULL));
	/****************** dynamic memory allocation ******************/
	solution = new int[n + 1];
	/************** generate random solution and run BLS algorithm **************/

	//long num_iterations = 100000;
	clock_t time;
	for (int restart = 1; restart <= max_restart; restart++)
	{
		generate_random_solution(n, solution);
		iteration = 0;
		BLS(n, a, b, c, solution, cost, num_iterations, best_objective, time);
		if (cost<best_cost)
		{
			best_cost = cost;
			cout << "Solution value " << cost << " " << (time) / static_cast<double>(CLOCKS_PER_SEC) << " " << 100.0*static_cast<double>(cost - best_objective) / static_cast<double>(best_objective) << endl;
		}
		totalcost += cost;
		totaltime += time;
	}
	cout << totalcost / max_restart << "and the average time is:" << totaltime / max_restart;
	delete[]solution;
}
