#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
using namespace std;
char filepath[100] = "test";//The folder in which the instances are stored
char QR1name[10] = "QR1-";
char typef[6][10] = { "","-Rgen","-NCR","-NCV","-PCR","-PCV" };
char range[2][10] = { "-S.txt","-L.txt" };
char QAPname[20];
int a_lower_bound[2] = { 1,1 };
int a_upper_bound[2] = { 10,50 };

int b_lower_bound[2] = { 10,50 };
int b_upper_bound[2] = { 20,100 };

int c_lower_bound[2] = { 20,100 };
int c_upper_bound[2] = { 40,400 };
int ahalf[2] = { 5,25 };
int bhalf[2] = { 15,75 };
void transpose(int & a, int & b) { long temp = a; a = b; b = temp; }
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
	//int x = n*n;
	quicksort(arr, 0, n - 1);
}

void decreasing_entries_generator(int* arr, int n)
{
	quicksort(arr, 0, n - 1);
	int arrColumnIndex = 0;
	int* ar = new int[n];
	for (int k = n - 1; k >= 0; k--)
	{
		ar[arrColumnIndex] = arr[k];
		arrColumnIndex++;

	}
	for (int i = 0; i<n; i++)
	{
		arr[i] = ar[i];
	}
	//display_one_dimensional_array(rowVector, n);
}
void generate_random_permutation(int n, int* p)
{
	int i;
	for (i = 0; i < n; i = i + 1) p[i] = i;
	for (i = 0; i < n - 1; i = i + 1)
	{
		transpose(p[i], p[i + rand() % (n - i)]);
	}
}
void generate_random_zeroORone(bool * zeroone, int* p, int n)
{
	for (int i = 0; i < n; i++)
	{
		zeroone[p[i]] = rand() % 2;
	}
}
void generate_QR1_Random(int n,int range_num, int **a, int **b, int**c) {
	two_dimensional_ranArray(a, n, a_lower_bound[range_num], a_upper_bound[range_num]);
	two_dimensional_ranArray(b, n, b_lower_bound[range_num], b_upper_bound[range_num]);
	two_dimensional_ranArray(c, n, c_lower_bound[range_num], c_upper_bound[range_num]);
}
void generate_QR1_Rgen(int n, int range_num, int **a, int **b, int**c) {
	generate_QR1_Random(n, range_num, a, b, c);
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			a[i][j] -= ahalf[range_num];
		}
	}
	for (int i = 0; i < n; i = i + 1)
	{
		for (int j = 0; j < n; j = j + 1)
		{
			b[i][j] -= bhalf[range_num];
		}
	}
}
void generate_QR1_NCR(int n, int range_num, int **a, int **b, int**c) {
	bool * zeroone = new bool[n];
	int * p = new int[n];
	generate_QR1_Random(n, range_num, a, b, c);
	srand(1);
	generate_random_permutation(n, p);
	generate_random_zeroORone(zeroone, p, n);
	
	for (int i = 0; i < n; i++)
	{
		if (zeroone[i] == 0)
		{
			increasing_entries_generator(a[i], n);
			decreasing_entries_generator(b[i], n);//NCRow
			for (int j = 0; j < n - 1; j++)
			{
				if (a[i][j] > a[i][j + 1])
				{
					cout << "is_increasing=" << false << endl;
					break;
				}
			}
		}
		else
		{
			decreasing_entries_generator(a[i], n);
			increasing_entries_generator(b[i], n);//NCRow
		}
	}
	delete[] zeroone;
	delete[] p;
}
void generate_QR1_NCV(int n, int range_num, int **a, int **b, int**c) {
	int * arr = new int[n*n];
	int * brr = new int[n*n];
	int * p = new int[n*n];
	int counter = 0;
	generate_QR1_Random(n, range_num, a, b, c);
	srand(1);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			arr[counter] = a[i][j];
			brr[counter] = b[i][j];
			counter++;
		}
	}
	increasing_entries_generator(arr, n*n);
	for (int i = 0; i < n*n - 1; i++)
	{
		if (arr[i] > arr[i + 1])
		{
			cout << "is_increasing=" << false << endl;
			break;
		}
	}
	decreasing_entries_generator(brr, n*n);
	generate_random_permutation(n*n, p);
	counter = 0;
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
	delete[] brr;
	delete[] arr;
	delete[] p;
}
void generate_QR1_PCR(int n, int range_num, int **a, int **b, int**c) {
	bool * zeroone = new bool[n];
	int * p = new int[n];
	generate_QR1_Random(n, range_num, a, b, c);
	srand(1);
	generate_random_permutation(n, p);
	generate_random_zeroORone(zeroone, p, n);
	for (int i = 0; i < n; i++)
	{
		if (zeroone[i] == 0)
		{
			increasing_entries_generator(a[i], n);
			increasing_entries_generator(b[i], n);
			for (int j = 0; j < n - 1; j++)
			{
				if (a[i][j] > a[i][j + 1])
				{
					cout << "is_increasing=" << false << endl;
					break;
				}
			}
		}
		else
		{
			decreasing_entries_generator(a[i], n);
			decreasing_entries_generator(b[i], n);
		}
	}
}
void generate_QR1_PCV(int n, int range_num, int **a, int **b, int**c) {
	int * arr = new int[n*n];
	int * brr = new int[n*n];
	int * p = new int[n*n];
	int counter = 0;
	generate_QR1_Random(n, range_num, a, b, c);
	srand(1);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			arr[counter] = a[i][j];
			brr[counter] = b[i][j];
			counter++;
		}
	}
	increasing_entries_generator(arr, n*n);
	for (int i = 0; i < n*n - 1; i++)
	{
		if (arr[i] > arr[i + 1])
		{
			cout << "is_increasing=" << false << endl;
			break;
		}
	}
	increasing_entries_generator(brr, n*n);
	generate_random_permutation(n*n, p);
	counter = 0;
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
	delete[] brr;
	delete[] arr;
	delete[] p;
}
void generate_QR1_QAP(int n, int range_num, int **a, int **b, int**c) {
	int aii, bii,n2;
	ifstream ReadFile;
	ReadFile.open(QAPname, ios::in);
	if (ReadFile.fail())//文件打开失败:返回0
	{
		printf("Open file failed, make sure the source QAP file exists\n");
	}
	else {
		ReadFile >> n2;
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
	}
	
}
void Read_File(char *filename,int &n, int **a, int **b, int **c) {
	ifstream read;
	read.open(filename, ios::in);
	read >> n;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			read >> a[i][j];
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			read >> b[i][j];
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			read >> c[i][j];
		}
	}
}
int main()
{
	int n, n1;
	int type_num = 1;
	int range_num = 1;
	int **a, **b, **c;
	int seed = 0;
	char infile[100]="";
	char outfile[100] = "";
	cout << "Seven classes of test problems:" << endl;
	cout << "1.QR1-Random" << endl;
	cout << "2.QR1-Rgen" << endl;
	cout << "3.QR1-NCR" << endl;
	cout << "4.QR1-NCV" << endl;
	cout << "5.QR1-PCR" << endl;
	cout << "6.QR1-PCV" << endl;
	cout << "7.QR1-QAP" << endl;
	cout << "Enter the number to choose the class of instances you want to generate (for example, 6 means the class QR1-PCV):";
	cin >> type_num;
	cout << endl;
	if (type_num != 7) {
		cout << "0:S (small range: Each of the entries in matrix a are limited to [1-10], matrix b are limited to [10-20] and matrix c are limited to [20-40])" << endl;
		cout << "1:L (large range: Each of the entries in matrix a are limited to [1-50], matrix b are limited to [50-100] and matrix c are limited to[100-400])" << endl;
		cout << "Enter the number: ";
		cin >> range_num;
		cout << "Enter the sise n of test instance: ";
		cin >> n;
		cout << endl;
		char numc[20] = "";
		itoa(n, numc, 10);
		strcat(outfile, filepath);
		strcat(outfile, "/");
		strcat(outfile, QR1name);
		strcat(outfile, numc);
		strcat(outfile, typef[type_num - 1]);
		strcat(outfile, range[range_num]);
	}
	else {
		cout << "Enter the source QAP instance file name (Make sure the source file exists. For example, tai20a.txt):";
		cin >> QAPname;
		char cc = ' ';
		n = 0;
		for (int i = 0; i < 20; i++) {
			cc = QAPname[i];
			if (cc <= '9'&&cc >= '0') {
				n = n * 10 + (cc - '0');
			}
		}
		cout << n << endl;
		strcat(outfile, filepath);
		strcat(outfile, "/");
		strcat(outfile, QR1name);
		strcat(outfile, QAPname);
	}
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
	void(*fun)(int n, int range_num, int **a, int **b, int**c);
	switch (type_num) {
	case 1:
		fun = generate_QR1_Random;
		break;
	case 2:
		fun = generate_QR1_Rgen;
		break;
	case 3:
		fun = generate_QR1_NCR;
		break;
	case 4:
		fun = generate_QR1_NCV;
		break;
	case 5:
		fun = generate_QR1_PCR;
		break;
	case 6:
		fun = generate_QR1_PCV;
		break;
	case 7:
		fun = generate_QR1_QAP;
		break;
	default:
		fun = generate_QR1_Random;
		break;
	}
	srand(1);
	fun(n, range_num, a, b, c);
	FILE *fp;
	errno_t err = fopen_s(&fp, outfile, "a");
	fprintf(fp, "%d\n", n);
	Display_Input_Instances(fp, a, n);
	Display_Input_Instances(fp, b, n);
	Display_Input_Instances(fp, c, n);
	fclose(fp);
	for (int i = 0; i < n; i++) {
		delete[] a[i];
		delete[] b[i];
		delete[] c[i];
	}
	delete[] a;
	delete[] b;
	delete[] c;
	system("pause");
	return 0;
}
