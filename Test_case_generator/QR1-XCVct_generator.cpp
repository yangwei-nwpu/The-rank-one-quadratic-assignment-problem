#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
using namespace std;
char * readfilename = "C:/Users/wei yang/Desktop/test/test50/r1800.txt";
char * outfilename = "C:/Users/wei yang/Desktop/test/test50/2/pr1800.txt";
void transpose(int & a, int & b) { long temp = a; a = b; b = temp; }
int partition(int* arr, int lowIndex, int highIndex)
{
	int pivotIndex = highIndex;

	while (lowIndex != highIndex)
	{
		if (arr[lowIndex] <= arr[pivotIndex])
		{
			lowIndex++;
		}

		if (arr[highIndex] >= arr[pivotIndex] && highIndex != lowIndex)
		{
			highIndex--;
		}

		if (arr[lowIndex] > arr[highIndex] && arr[highIndex] < arr[pivotIndex])
		{
			//swap arr[lowIndex] with arr[highIndex]
			int low_index_value = arr[lowIndex];
			arr[lowIndex] = arr[highIndex];
			arr[highIndex] = low_index_value;
			//lowIndex += 1;
			//highIndex -= 1;
		}
	}

	//now lowIndex = highIndex
	if (arr[lowIndex] >= arr[pivotIndex])
	{
		//swap arr[lowIndex] with arr[piviotIndex]
		int low_index_value = arr[lowIndex];
		arr[lowIndex] = arr[pivotIndex];
		arr[pivotIndex] = low_index_value;

		return lowIndex;
	}
	else
	{
		//swap arr[lowIndex+1] with arr[pivotIndex]
		int low_index_value = arr[lowIndex + 1];
		arr[lowIndex + 1] = arr[pivotIndex];
		arr[pivotIndex] = low_index_value;

		return lowIndex + 1;
	}
}

void quicksort(int* arr, int lowIndex, int highIndex)
{
	int pivotIndex;

	if (lowIndex < highIndex)
	{
		pivotIndex = partition(arr, lowIndex, highIndex);
		quicksort(arr, lowIndex, pivotIndex - 1);
		quicksort(arr, pivotIndex + 1, highIndex);
	}
}


void increasing_entries_generator(int* arr, int n)
{
	int x = n*n;
	quicksort(arr, 0, x - 1);
}

// Generate random entries for arr in decreasing order
// PRE:
// PARAM: arr - one dimensional array, n - # of entries of arr;
// POST:  entries of arr are in decreasing order
void decreasing_entries_generator(int* arr, int n)
{
	//extract each row and then use quicksort to sort each row's entry in increasing order
	int x = n*n;
	quicksort(arr, 0, x - 1);
	int arrColumnIndex = 0;
	int* ar = new int[x];
	for (int k = x - 1; k >= 0; k--)
	{
		ar[arrColumnIndex] = arr[k];
		arrColumnIndex++;

	}
	for (int i = 0; i<x; i++)
	{
		arr[i] = ar[i];
	}
	//display_one_dimensional_array(rowVector, n);
}
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
void generate_random_permutation(long n, int* p)
{
	int i;
	long x = n*n;
	for (i = 0; i < x; i = i + 1) p[i] = i;
	for (i = 0; i < x - 1; i = i + 1)
	{
		transpose(p[i], p[i + rand() % (x - i)]);
	}
	bool istrue = func(p, x);
	cout << "istrue=" << istrue << endl;
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
	ReadFile.open(readfilename, ios::in);
	ReadFile >> n;
	ReadFile >> best_objective;
	cout << n << " " << best_objective << endl;
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
	int * arr = new int[n*n];
	int * brr = new int[n*n];
	int * p = new int[n*n];
	for (int m = 0; m < n*n; m = m + 1)
	{
		ReadFile >> arr[m];
	}
	for (int m = 0; m < n*n; m = m + 1)
	{
		ReadFile >> brr[m];
	}
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			ReadFile >> c[i][j];
		}
	}
	ReadFile.close();
	increasing_entries_generator(arr, n);
	for (int i = 0; i < n*n - 1; i++)
	{
		//temp = arr[i];
		if (arr[i] > arr[i + 1])
		{
			cout << "is_increasing=" << false << endl;
			break;
		}
	}
	increasing_entries_generator(brr, n);//PCVct
	//decreasing_entries_generator(brr, n);//NCVct
	
	generate_random_permutation(n, p);
	int counter = 0;
	for (int i = 0; i <n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			a[i][j] = arr[p[counter]];
			counter++;
		}
	}
	counter = 0;
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			b[i][j] = brr[p[counter]];
			counter++;
		}
	}

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
