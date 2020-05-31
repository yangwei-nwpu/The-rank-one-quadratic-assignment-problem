#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
using namespace std;
char * readfilename = "C:/Users/wei yang/Desktop/QAP/qapdata/wil100.txt";
char * outfilename = "C:/Users/wei yang/Desktop/test/QAP/wil100.txt";
void transpose(int & a, int & b) { long temp = a; a = b; b = temp; }
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
	ReadFile.open(readfilename, ios::in);
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
	int aii, bii, cii;;
	for (int i = 0; i < n; i = i + 1)
	{
		aii = 0;
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> a[i][j];
			aii += a[i][j];
		}
		a[i][i] = aii / n + 1;
	}
	for (int i = 0; i < n; i = i + 1)
	{
		bii = 0;
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> b[i][j];
			bii += b[i][j];
		}
		b[i][i] = bii / n + 1;
	}
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			c[i][j] = 0;
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
