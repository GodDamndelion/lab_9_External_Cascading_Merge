#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

using namespace std;

struct STRYCT
{
	int i;
	char c;
	bool operator>(const STRYCT& obj);
	bool operator<(const STRYCT& obj);
	bool operator<=(const STRYCT& obj);
};

const int n = 6;
const int count_of_elements = 1000;

struct Sequence
{
	std::fstream file;
	STRYCT elem;
	bool eof, eor;
	void ReadNext();
	void StartRead(std::string filename);
	void StartWrite(std::string filename);
	void Close();
	void Copy(Sequence& x);
	void CopyRun(Sequence& x);
	int CountSeries(); //Добавлено для начальной цифры (и для distr)
};

void randomize_file(std::ofstream& file);
int sum_arr(int* a, int n);
void print_arr(int* a, int n);
void reverse_arr(int* a, int n);

void allocation(string name, fstream*& files, Sequence*& seqs, int* final_arr, int n);
void merge(Sequence& f0, Sequence& f1, Sequence& f2);
void merge_once(Sequence* seqs, int n, int* allocations, int* t);
void cycle_of_merge(Sequence* seqs, int n, int* allocations, int* t, int k);
void mega_merge(Sequence* seqs, int n, int* allocations);
void cascade_sort(string name);
void remove_files();

int main()
{
	//SetConsoleOutputCP(1251);
	//SetConsoleCP(1251);
	string filename = "original_file.txt";
	std::ofstream original_file(filename, ios::binary);
	randomize_file(original_file);
	original_file.close();
	cascade_sort(filename);
	remove_files();

	cin.get();
	return 0;
}

//================================================================

bool STRYCT::operator>(const STRYCT& obj)
{
	return i > obj.i || (i == obj.i && c > obj.c);
}
bool STRYCT::operator<(const STRYCT& obj)
{
	return i < obj.i || (i == obj.i && c < obj.c);
}
bool STRYCT::operator<=(const STRYCT& obj)
{
	return i < obj.i || (i == obj.i && c <= obj.c);
}
void Sequence::ReadNext()
{
	eof = file.eof();
	if (!eof)
	{
		file.read((char*)&elem, sizeof elem);
	}
}
void Sequence::StartRead(std::string filename)
{
	file.open(filename, ios::binary | std::ios::in);
	ReadNext();
	eor = eof;
}
void Sequence::StartWrite(std::string filename)
{
	file.open(filename, ios::binary | std::ios::out);
}
void Sequence::Close()
{
	file.close();
}
void Sequence::Copy(Sequence& x)
{
	if (!x.file.eof())
	{
		elem = x.elem;
		file.write((char*)&elem, sizeof elem);
		//cout << elem.i << '[' << elem.c << "] ";
	}
	x.ReadNext();
	x.eor = x.eof || x.elem < elem;
}
void Sequence::CopyRun(Sequence& x)
{
	do
	{
		Copy(x);
	} while (!x.eor);
}
int Sequence::CountSeries()
{
	int counter = 0;
	while (!eof)
	{
		do
		{
			STRYCT elem1 = elem;
			ReadNext();
			eor = eof || elem < elem1;
		} while (!eor);
		counter++;
	}
	return counter;
}

void randomize_file(std::ofstream& file)
{
	srand(time(0));
	int i;
	char c;
	//
	//int a[count_of_elements] = { 21, -65, 41, 53, -61, -26, 70, 25, 96, 4 };
	//int a[count_of_elements] = { -66, 4, - 13, 99, - 7, - 79, 77, 10, 11, - 26 };
	//
	for (int j = 0; j < count_of_elements; ++j)
	{
		i = rand() % 201 - 100;
		//i = a[j];
		c = rand() % ('Z' - 'A' + 1) + 'A';
		STRYCT s;
		s.i = i;
		s.c = c;
		file.write((char*)&s, sizeof s);
		//cout << s.i << '[' << s.c << "] ";
	}
	//cout << '\n';
}
int sum_arr(int* a, int n)
{
	int sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum += a[i];
	}
	return sum;
}
void print_arr(int* a, int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << a[i] << ' ';
	}
	cout << '\n';
}
void reverse_arr(int* a, int n)
{
	if (n % 2 == 0)
	{
		for (int i = 0; i <= (n - 1) / 2; i++)
		{
			swap(a[i], a[n - i - 1]);
		}
	}
	else
	{
		for (int i = 0; i <= n / 2; i++)
		{
			swap(a[i], a[n - i - 1]);
		}
	}
}
/*
void copy(int*& a, int* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		a[i] = b[i];
	}
}
*/

void allocation(string name, fstream*& files, Sequence*& seqs, int* final_arr, int n)
{
	Sequence f0;
	f0.StartRead(name);
	for (int i = 0; i < n; i++)
	{
		seqs[i].StartWrite("f" + to_string(i + 1) + ".txt");
	}
	int k = f0.CountSeries();
	f0.Close();
	f0.StartRead(name);
	cout << k << endl << endl;
	int* a = new int[n];
	for (int i = 0; i < n - 1; i++)
	{
		a[i] = 1;
	}
	a[n - 1] = 0;

	while (sum_arr(a, n - 1) < k)
	{
		int* b = new int[n];
		for (int i = 0; i < n; i++)
		{
			b[i] = a[i];
		}
		for (int i = 0; i < n - 1; i++)
		{
			a[i] = sum_arr(b, n - i - 1);
		}
	}
	//print_arr(a, n);
	if (sum_arr(a, n - 1) == k)
	{
		for (int i = 0; i < n; i++)
		{
			final_arr[i] = 0;
		}
		for (int i = 0; i < n; i++)
		{
			int counter = 0;
			while (!f0.file.eof() && counter != a[i])
			{
				seqs[i].CopyRun(f0);
				++final_arr[i];
				counter++;
			}
		}
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			final_arr[i] = 0;
		}
		for (int i = 0; i < n; i++)
		{
			int counter = 0;
			while (!f0.file.eof() && counter != a[i])
			{
				seqs[i].CopyRun(f0);
				++final_arr[i];
				counter++;
			}
		}
		for (int i = 0; i < n - 1; i++)
		{
			while (final_arr[i] != a[i])
			{
				++final_arr[i];
				STRYCT s;
				s.i = INT_MAX;
				s.c = 'Z';
				seqs[i].file.write((char*)&s, sizeof s);
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		seqs[i].Close();
	}
	f0.Close();
	print_arr(final_arr, n);
	cout << endl;
}
void merge(Sequence& f0, Sequence& f1, Sequence& f2)
{
	//cout << '\n';
	while (!f1.eor && !f2.eor)
	{
		if (f1.elem <= f2.elem)
		{
			f0.Copy(f1);
			//cout << f1.elem.i << '[' << f1.elem.c << "] ";
		}
		else
		{
			f0.Copy(f2);
			//cout << f1.elem.i << '[' << f1.elem.c << "] ";
		}
	}
	if (!f1.eor && !f1.eof)
	{
		f0.CopyRun(f1);
	}
	if (!f2.eor && !f2.eof)
	{
		f0.CopyRun(f2);
	}
	//cout << '\n';
}
void merge_once(Sequence* seqs, int n, int* allocations, int* t)
{

	//cout << endl;
	Sequence tmp;
	tmp.StartWrite("tmp.txt");
	int p = n;
	merge(tmp, seqs[t[0]], seqs[t[1]]);
	allocations[0]--;
	allocations[1]--;
	Sequence tmp2;
	tmp.Close();
	for (int i = 2; i < p - 1; i++)
	{
		tmp2.StartWrite("tmp2.txt");
		tmp.StartRead("tmp.txt");
		merge(tmp2, tmp, seqs[t[i]]);

		tmp2.Close();
		tmp.Close();

		remove("tmp.txt");
		tmp2.StartRead("tmp2.txt");
		tmp.StartWrite("tmp.txt");

		while (!tmp2.eof)
		{
			tmp.CopyRun(tmp2);
		}
		tmp.Close();
		tmp2.Close();
		remove("tmp2.txt");
		allocations[i]--;
	}

	//tmp.file.open("tmp.txt", ios::binary | ios::in);

	//while (!tmp.file.eof())
	//{
	//	STRYCT s;
	//	tmp.file.read((char*)&s, sizeof s);
	//	seqs[t[n - 1]].file.write((char*)&s, sizeof s);
	//}

	//cout << '\n';
	tmp.StartRead("tmp.txt");
	while (!tmp.eof)
	{
		seqs[t[n - 1]].CopyRun(tmp);
	}

	tmp.Close();
	remove("tmp.txt");
	allocations[p - 1]++;
	//cout << endl << endl;
	//print_arr(allocations, n);
}
void cycle_of_merge(Sequence* seqs, int n, int* allocations, int* t, int k)
{
	for (int i = 0; i < k; i++)
	{
		merge_once(seqs, n, allocations, t);
		for (int i = 0; i < n; i++)
		{
			seqs[t[i]].eor = false;
		}
	}
}
void mega_merge(Sequence* seqs, int n, int* allocations)
{
	int* t = new int[n];
	for (int i = 0; i < n; i++)
	{
		t[i] = i;
	}
	while (allocations[0] > 1)
	{
		for (int i = 0; i < n - 1; i++)
		{
			seqs[t[i]].StartRead("f" + to_string(t[i] + 1) + ".txt");
		}
		for (int i = n; i > 2; i--)
		{
			seqs[t[i - 1]].file.open("f" + to_string(t[i - 1] + 1) + ".txt", ios::binary | ios::out);
			cycle_of_merge(seqs, i, allocations, t, allocations[i - 2]);
			//cout << '\n';
			print_arr(allocations, n);
			string path = "f" + to_string(t[i - 2] + 1) + ".txt";
			seqs[t[i - 2]].StartWrite(path);
			seqs[t[i - 2]].Close();
		}
		seqs[t[1]].StartWrite("f" + to_string(t[1] + 1) + ".txt");
		//seqs[t[1]].StartWrite("f2.txt");
		while (!seqs[t[0]].eof)
		{
			seqs[t[1]].CopyRun(seqs[t[0]]);
		}
		//copy_Kser(seqs[t[0]], seqs[t[1]]);
		seqs[t[0]].Close();
		string path = "f" + to_string(t[0] + 1) + ".txt";
		remove(path.c_str());
		seqs[t[0]].StartWrite(path);
		seqs[t[0]].Close();
		allocations[1] += allocations[0];
		allocations[0] = 0;

		for (int i = 0; i < n; i++)
		{
			seqs[t[i]].Close();
		}

		reverse_arr(t, n);
		reverse_arr(allocations, n);
		print_arr(allocations, n);
		//print_arr(t, n);
	}
	for (int i = 0; i < n - 1; i++)
	{
		seqs[t[i]].StartRead("f" + to_string(t[i] + 1) + ".txt");
	}
	seqs[t[n - 1]].file.open("f" + to_string(t[n - 1] + 1) + ".txt", ios::binary | ios::out);
	cycle_of_merge(seqs, n, allocations, t, 1);
	for (int i = 0; i < n; i++)
	{
		seqs[t[i]].Close();
	}

	fstream result_file("result.txt", ios::out); //Не бинарный!
	string result_string;

	//seqs[t[n - 1]].file.open("f" + to_string(t[n - 1] + 1) + ".txt");
	//while (!seqs[t[n - 1]].file.eof())
	//{
	//	STRYCT s;
	//	seqs[t[n - 1]].file.read((char*)&s, sizeof s);
	//	result_string += to_string(s.i) + '[' + s.c + "] ";
	//}

	seqs[t[n - 1]].StartRead("f" + to_string(t[n - 1] + 1) + ".txt");
	while (!seqs[t[n - 1]].eof)
	{
		STRYCT s;
		do
		{
			if (!seqs[t[n - 1]].file.eof())
			{
				s = seqs[t[n - 1]].elem; /////////////
				result_string += to_string(s.i) + '[' + s.c + "] "; //////
				//cout << s.i << '[' << s.c << "] ";
			}
			seqs[t[n - 1]].ReadNext();
			seqs[t[n - 1]].eor = seqs[t[n - 1]].eof || seqs[t[n - 1]].elem < s;
		} while (!seqs[t[n - 1]].eor);
	}
	//cout << '\n';

	//tmp.StartRead("tmp.txt");
	//while (!tmp.eof)
	//{
	//	seqs[t[n - 1]].CopyRun(tmp);
	//}

	int k = result_string.find(to_string(INT_MAX));
	result_string = result_string.substr(0, k);
	result_file << result_string;
	print_arr(allocations, n);
	//print_arr(t, n);
	for (int i = 0; i < n; i++)
	{
		seqs[t[i]].Close();
	}
	cout << "\n" << result_string << "\n";
}
void cascade_sort(string name)
{
	fstream* files = new fstream[n];
	for (int i = 0; i < n; i++)
	{
		files[i].open("f" + to_string(i + 1) + ".txt", ios::binary | ios::out);
	}
	int* final_arr = new int[n];
	Sequence* seqs = new Sequence[n];
	allocation(name, files, seqs, final_arr, n);
	int* t = new int[n];
	for (int i = 0; i < n; i++)
	{
		t[i] = i;
	}
	mega_merge(seqs, n, final_arr);
	for (int i = 0; i < n; i++)
	{
		seqs[t[i]].Close();
	}
	delete[]files;
}
void remove_files()
{
	for (int i = 0; i < n; i++)
	{
		string path = ("f" + to_string(i + 1) + ".txt");
		remove(path.c_str());
	}
}