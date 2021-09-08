#pragma once
using namespace std;
template <typename T>
class Matrix
{
private:
	T * * M;
	int row;
	int col;
public:
	Matrix();
	Matrix(int r, int c);
	Matrix(const Matrix& cpy);
	T get_el(int i, int j) const;
	void set_el(int i, int j, T value);
	Matrix& operator=(const Matrix& cpy);
	Matrix operator+(const Matrix& right) const;
	Matrix operator-(const Matrix& right) const;
	Matrix operator*(const Matrix& right) const;
	friend ostream& operator<<(ostream &os, const Matrix<T> &c)
	{
		if ((c.row == 0) && (c.col == 0)) 
		{
			os << "Matrix is empty" << '\n';
			return os;
		}
		else
		{
			os << "----------------" << '\n';
			for (int i = 0; i < c.row; i++) 
			{
				for (int j = 0; j < c.col; j++) 
				{
					os << c.get_el(i, j) << '\t';
				}
				os << '\n';
			}
			os << "----------------" << '\n';
			return os;
		}
	}

	~Matrix();
};

template <typename T>
Matrix<T>::Matrix()
{
	row = 0;
	col = 0;
	M = nullptr;
	cout << "construct" << '\n';
};

template <typename T>
Matrix<T>::Matrix(int r, int c)
{
	row = r;
	col = c;
	M = (T**) new T*[row];
	for (int i = 0; i < row; i++) {
		M[i] = (T*) new T[col];
	}
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			M[i][j] = 0;
		}
	}
	cout << "construct with rows " << r << " and with columns " << c << '\n';
};

template <typename T>
Matrix<T>::Matrix<T>(const Matrix<T>& cpy)
{
	this->row = cpy.row;
	this->col = cpy.col;
	this->M = (T**) new T*[this->row];
	for (int i = 0; i < this->row; i++)
		this->M[i] = (T*) new T[this->col];
	for (int i = 0; i < this->row; i++)
	{
		for (int j = 0; j < this->col; j++) {
			this->set_el(i, j, cpy.get_el(i, j));
		}
	}
}

template <typename T>
T Matrix<T>::get_el(int i, int j) const
{
	if ((i < this->row) && (i >= 0))
		if ((j < this->col) && (j >= 0))
			return M[i][j];
	cout << "Invalid indexes" << '\n';
	system("pause");
	exit(1);
}

template <typename T>
void Matrix<T>::set_el(int i, int j, T value)
{
	if ((i < this->row) && (i >= 0))
	{
		if ((j < this->col) && (j >= 0))
		{
			M[i][j] = value;
			return;
		}
	}
	cout << "Invalid indexes" << '\n';
	system("pause");
	exit(1);
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& cpy)
{
	for (int i = 0; i < this->row; i++)
		delete[] this->M[i];
	delete[] this->M;
	this->row = cpy.row;
	this->col = cpy.col;
	this->M = (T**) new T*[this->row];
	for (int i = 0; i < this->row; i++)
		this->M[i] = (T*) new T[this->col];
	for (int i = 0; i < this->row; i++)
	{
		for (int j = 0; j < this->col; j++) {
			this->set_el(i, j, cpy.get_el(i, j));
		}
	}
	return *this;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& right) const
{
	if ((this->row != right.row) || (this->col != right.col))
	{
		cout << "Matrixes has different size " << '\n';
		Matrix<T> c;
		return c;
	}
	else
	{
		Matrix<T> c(this->row, this->col);
		for (int i = 0; i < c.row; i++)
		{
			for (int j = 0; j < c.col; j++) {
				c.set_el(i, j, this->get_el(i, j) + right.get_el(i, j));
			}
		}
		return c;
	}
}

template <typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& right) const
{
	if ((this->row != right.row) || (this->col != right.col))
	{
		cout << "Matrixes has different size " << '\n';
		Matrix<T> c;
		return c;
	}
	else
	{
		Matrix<T> c(this->row, this->col);
		for (int i = 0; i < c.row; i++)
		{
			for (int j = 0; j < c.col; j++) {
				c.set_el(i, j, this->get_el(i, j) - right.get_el(i, j));
			}
		}
		return c;
	}
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& right) const
{
	if (this->col != right.row)
	{
		cout << "Invalid sizes of matrixes. Can't be multiplied!" << '\n';
		Matrix<T> c;
		return c;
	}
	else
	{
		Matrix<T> c(this->row, right.col);
		T sum = 0;
		for (int i = 0; i < c.row; i++)
		{
			for (int j = 0; j < c.col; j++)
			{
				for (int n = 0; n < this->col; n++)
				{
					sum += this->get_el(i, n) * right.get_el(n, j);
				}
				c.set_el(i, j, sum);
				sum = 0;
			}
		}
		return c;
	}
}

template <typename T>
Matrix<T>::~Matrix()
{
	for (int i = 0; i < row; i++)
		delete[] M[i];
	delete[] M;
	cout << "destruct" << '\n';
};