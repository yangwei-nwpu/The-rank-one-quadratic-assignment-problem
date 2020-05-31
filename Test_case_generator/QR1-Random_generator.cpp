#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
using namespace std;
char * outfilename = "C:/Users/wei yang/Desktop/MAP/test1to10/r500.txt";
void two_dimensional_ranArray(int** arr, int n, int low, int up)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			arr[i][j] = rand() % (up - low + 1) + low;

		}
	}

}
void Display_Input_Instances(FILE *fp, int **arr, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			fprintf(fp, "%d ", arr[i][j]);
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");

}

int main()
{
	int n;
	long best_objective;
	int **a, **b, **c;
	int seed = 0;
	cin >> n >> best_objective;

	int lower_bound, upper_bound;
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

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a[i][j] = 0;
			b[i][j] = 0;
			c[i][j] = 0;
		}
	}


	int a_lower_bound = 1;
	int a_upper_bound = 10;

	int b_lower_bound = 10;
	int b_upper_bound = 20;

	int c_lower_bound = 20;
	int c_upper_bound = 40;
	//seed = 5;
	//srand(seed);
	lower_bound = a_lower_bound;
	upper_bound = a_upper_bound;
	two_dimensional_ranArray(a, n, lower_bound, upper_bound);
	
	lower_bound = b_lower_bound;
	upper_bound = b_upper_bound;
	two_dimensional_ranArray(b, n, lower_bound, upper_bound);
	
	lower_bound = c_lower_bound;
	upper_bound = c_upper_bound;
	two_dimensional_ranArray(c, n, lower_bound, upper_bound);
	
	FILE *fp;
	errno_t err= fopen_s(&fp,outfilename, "a+");
	fprintf(fp, "%d  ", n);
	fprintf(fp, "%ld\n", best_objective);
	Display_Input_Instances(fp, a, n);
	Display_Input_Instances(fp, b, n);
	Display_Input_Instances(fp, c, n);

	fclose(fp);

	return 0;

}
