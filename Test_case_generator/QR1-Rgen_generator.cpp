#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
using namespace std;
char * readfilename = "C:/Users/wei yang/Desktop/test/test10/QR1-200-R-S.txt";
char * outfilename = "C:/Users/wei yang/Desktop/test/test10/6/QR1-200-Rgen-S.txt";
void transpose(int & a, int & b) { long temp = a; a = b; b = temp; }
int ahalf = 5, bhalf = 15;
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
	ifstream ReadFile;
	string temp;
	ReadFile.open(readfilename, ios::in);//ios::in 表示以只读的方式读取文
	ReadFile >> n;
	ReadFile >> best_objective;
	cout << n << " " << best_objective << endl;
	a = new int*[n];
	b = new int*[n];
	c = new int*[n];

	for (int i = 0; i < n; i++)
	{
		a[i] = new int[n];
		b[i] = new int[n];
		c[i] = new int[n];
	}
	int * arr = new int[n*n];
	int * brr = new int[n*n];
	int * p = new int[n*n];
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> a[i][j];
			a[i][j] -= ahalf;
		}
	}
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> b[i][j];
			b[i][j] -= bhalf;
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
	FILE *fp;
	errno_t err = fopen_s(&fp, outfilename, "a+");
	fprintf(fp, "%d  ", n);
	fprintf(fp, "%ld\n", best_objective);
	Display_Input_Instances(fp, a, n);
	Display_Input_Instances(fp, b, n);
	Display_Input_Instances(fp, c, n);

	fclose(fp);
	//system("pause");
	return 0;

}
