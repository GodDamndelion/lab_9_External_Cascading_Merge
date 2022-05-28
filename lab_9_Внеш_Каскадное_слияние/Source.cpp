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

const int n = 6;
const int count_of_elements = 10;

struct Sequence
{
	std::fstream file;
	STRYCT elem;
	bool eof, eor;
	void ReadNext()
	{
		eof = file.eof();
		if (!eof)
		{
			file.read((char*)&elem, sizeof(STRYCT));
		}
	}
	void StartRead(std::string filename)
	{
		file.open(filename, ios::binary | std::ios::in);
		ReadNext();
		eor = eof;
	}
	void StartWrite(std::string filename)
	{
		file.open(filename, ios::binary | std::ios::out);
	}

	void Close()
	{
		file.close();
	}

	void Copy(Sequence& x)
	{
		if (!x.file.eof())
		{
			elem = x.elem;
			file.write((char*)&elem, sizeof(STRYCT));
		}
		x.ReadNext();
		x.eor = x.eof || x.elem < elem;
	}
	void CopyRun(Sequence& x)
	{
		do
		{
			Copy(x);
		} while (!x.eor);
	}

	int CountSeries()
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
};



void CreateFile_(std::string filename)
{
	std::ofstream file(filename, ios::binary);
	srand(time(0));
	int i;
	char c;
	for (int j = 0; j < count_of_elements; ++j)
	{
		i = rand() % 201 - 100;
		c = rand() % ('Z' - 'A' + 1) + 'A';
		STRYCT s;
		s.i = i;
		s.c = c;
		file.write((char*)&s, sizeof(STRYCT));
		cout << s.i << '[' << s.c << "] ";
	}
	cout << '\n';
	file.close();
}

/*bool CheckFile(std::string filename)
{
	std::ifstream file(filename);
	int x, y;
	bool check = true;
	file >> y;
	while (!file.eof() && check)
	{
		x = y;
		file >> y;
		check = x <= y;
	}
	file.close();
	return check;
}*/


//=======================================

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
		cout << a[i] << '\t';
	}
	cout << endl;
}
void copy(int*& a, int* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		a[i] = b[i];
	}
}

//=================================================

void distribution(string name, fstream*& files, Sequence*& seqs, int* final_arr, int n)
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
	int* a1 = new int[n];
	int* a2 = new int[n];
	for (int i = 0; i < n - 1; i++)
	{
		a1[i] = 0;
		a2[i] = 1;
	}
	a1[0] = 1;
	a1[n - 1] = 0;
	a2[n - 1] = 0;

	while (sum_arr(a2, n - 1) < k)
	{
		int* b = new int[n];
		for (int i = 0; i < n; i++)
		{
			b[i] = a2[i];
		}
		for (int i = 0; i < n - 1; i++)
		{
			a2[i] = sum_arr(a1, n - i - 1);
		}
		for (int i = 0; i < n; i++)
		{
			a1[i] = b[i];
		}
	}
	print_arr(a2, n);
	if (sum_arr(a2, n - 1) == k)
	{
		for (int i = 0; i < n; i++)
		{
			final_arr[i] = 0;
		}
		for (int i = 0; i < n; i++)
		{
			int counter = 0;
			while (!f0.file.eof() && counter != a2[i])
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
			while (!f0.file.eof() && counter != a2[i])
			{
				seqs[i].CopyRun(f0);
				++final_arr[i];
				counter++;
			}
		}
		for (int i = 0; i < n - 1; i++)
		{
			while (final_arr[i] != a2[i])
			{
				++final_arr[i];
				STRYCT s;
				s.i = INT_MAX;
				s.c = 'Z';
				seqs[i].file.write((char*)&s, sizeof(STRYCT));
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




void MergeUnit(Sequence& f0, Sequence& f1, Sequence& f2)
{
	while (!f1.eor && !f2.eor)
	{
		if (f1.elem <= f2.elem)
		{
			f0.Copy(f1);
		}
		else
		{
			f0.Copy(f2);
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
}

void MergeOne(Sequence* seqs, int n, int* distribute, int* t)
{

	cout << endl;
	Sequence current;
	current.StartWrite("cur.txt");
	int p = n;
	MergeUnit(current, seqs[t[0]], seqs[t[1]]);
	distribute[0]--;
	distribute[1]--;
	Sequence sec_cur;
	current.Close();
	for (int i = 2; i < p - 1; i++)
	{
		sec_cur.StartWrite("sec_cur.txt");
		current.StartRead("cur.txt");
		MergeUnit(sec_cur, current, seqs[t[i]]);

		sec_cur.Close();
		current.Close();

		remove("cur.txt");
		sec_cur.StartRead("sec_cur.txt");
		current.StartWrite("cur.txt");

		while (!sec_cur.eof)
		{
			current.CopyRun(sec_cur);
		}
		current.Close();
		sec_cur.Close();
		remove("sec_cur.txt");
		distribute[i]--;
	}

	current.file.open("cur.txt", ios::binary | ios::in);
	while (!current.file.eof())
	{
		STRYCT s;
		current.file.read((char*)&s, sizeof(STRYCT));
		seqs[t[n - 1]].file.write((char*)&s, sizeof(STRYCT));
	}

	current.Close();
	remove("cur.txt");
	distribute[p - 1]++;
	cout << endl << endl;
	print_arr(distribute, n);
}


void MergeK(Sequence* seqs, int n, int* distribute, int* t, int k)
{
	for (int i = 0; i < k; i++)
	{
		MergeOne(seqs, n, distribute, t);
		for (int i = 0; i < n; i++)
		{
			seqs[t[i]].eor = false;
		}
	}
}

/*
void copy_Kser(Sequence& s1, Sequence& s2)
{
	s2.StartWrite("f2.txt");
	while (!s1.eof)
	{
		s2.CopyRun(s1);
	}
}*/

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
void CascadeMerge(Sequence* seqs, int n, int* distribute)
{
	int* t = new int[n];
	for (int i = 0; i < n; i++)
	{
		t[i] = i;
	}
	while (distribute[0] > 1)
	{
		for (int i = 0; i < n - 1; i++)
		{
			seqs[t[i]].StartRead("f" + to_string(t[i] + 1) + ".txt");
		}
		for (int i = n; i > 2; i--)
		{
			seqs[t[i - 1]].file.open("f" + to_string(t[i - 1] + 1) + ".txt", ios::binary | ios::out);
			MergeK(seqs, i, distribute, t, distribute[i - 2]);
			print_arr(distribute, n);
			string path = "f" + to_string(t[i - 2] + 1) + ".txt";
			seqs[t[i - 2]].StartWrite(path);
			seqs[t[i - 2]].Close();
		}
		seqs[t[1]].StartWrite("f" + to_string(t[1] + 1) + ".txt");
		while (!seqs[t[0]].eof)
		{
			seqs[t[1]].CopyRun(seqs[t[0]]);
		}
		seqs[t[0]].Close();
		string path = "f" + to_string(t[0] + 1) + ".txt";
		remove(path.c_str());
		seqs[t[0]].StartWrite(path);
		seqs[t[0]].Close();
		distribute[1] += distribute[0];
		distribute[0] = 0;

		for (int i = 0; i < n; i++)
		{
			seqs[t[i]].Close();
		}

		reverse_arr(t, n);
		reverse_arr(distribute, n);
		print_arr(distribute, n);
		print_arr(t, n);
		for (int i = 0; i < n; i++)
		{
			seqs[t[i]].Close();
		}
	}
	for (int i = 0; i < n - 1; i++)
	{
		seqs[t[i]].StartRead("f" + to_string(t[i] + 1) + ".txt");
	}
	seqs[t[n - 1]].file.open("f" + to_string(t[n - 1] + 1) + ".txt", ios::binary | ios::out);
	MergeK(seqs, n, distribute, t, 1);
	for (int i = 0; i < n; i++)
	{
		seqs[t[i]].Close();
	}

	fstream result_file("result.txt", ios::out); //Не бинарный!
	string result_string;
	seqs[t[n - 1]].file.open("f" + to_string(t[n - 1] + 1) + ".txt");
	while (!seqs[t[n - 1]].file.eof())
	{
		STRYCT s;
		seqs[t[n - 1]].file.read((char*)&s, sizeof(STRYCT));
		result_string += to_string(s.i) + '[' + s.c + "] ";
	}
	//int k = result_string.find(to_string(INT_MAX));
	//result_string = result_string.substr(0, k);
	result_file << result_string;
	print_arr(distribute, n);
	print_arr(t, n);
	for (int i = 0; i < n; i++)
	{
		seqs[t[i]].Close();
	}
	cout << "\n" << result_string << "\n";
}

void CascadeSort(string name)
{
	fstream* files = new fstream[n];
	for (int i = 0; i < n; i++)
	{
		files[i].open("f" + to_string(i + 1) + ".txt", ios::binary | ios::out);
	}
	int* final_arr = new int[n];
	Sequence* seqs = new Sequence[n];
	distribution(name, files, seqs, final_arr, n);
	int* t = new int[n];
	for (int i = 0; i < n; i++)
	{
		t[i] = i;
	}
	CascadeMerge(seqs, n, final_arr);
	for (int i = 0; i < n; i++)
	{
		seqs[t[i]].Close();
	}
	delete[]files;
}

void RemoveFiles()
{
	for (int i = 0; i < n; i++)
	{
		string path = ("f" + to_string(i + 1) + ".txt");
		remove(path.c_str());
	}
}

int main()
{
	CreateFile_("f0.txt");
	CascadeSort("f0.txt");
	RemoveFiles();

	cin.get();
	return 0;
}