#pragma once
using namespace std;
template <typename T>
class Matrix
{
private:
	T ** M;
	int row;
	int col;
public:
	Matrix();
	Matrix(int r, int c);
	Matrix(const Matrix& cpy);
	T get_el(int i, int j) const;
	int get_row() const;
	int get_col() const;
	void set_el(int i, int j, T value);
	Matrix& operator=(const Matrix& cpy);
	Matrix operator+(const Matrix& right) const;
	Matrix operator-(const Matrix& right) const;
	Matrix operator*(const Matrix& right) const;
	Matrix operator*(const T right) const;
	friend Matrix<T> operator*(const T left, const Matrix& right)
	{
		return right*left;
	}
	Matrix operator^(int q) const;
	Matrix& Gauss();
	Matrix& Tran();
	T det() const;
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
					os << round(c.get_el(i, j) * 1000) / 1000 << '\t';
				}
				os << '\n';
			}
			os << "----------------" << '\n';
			return os;
		}
	}
	friend istream& operator>>(istream &is, Matrix<T> &c)
	{
		cout << "Enter Matrix size " << '\n';
		cout << "rows " << '\t';
		int row;
		is >> row;
		cout << "columns " << '\t';
		int col;
		is >> col;
		cout << "Enter elements " << '\n';
		Matrix<T> d(row, col);
		c = d;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				cout << "M[" << i << "][" << j << "] = ";
				T value;
				is >> value;
				c.set_el(i, j, value);
			}
			cout << '\n';
		}
		return is;
	}
	~Matrix();
};

template <typename T>
Matrix<T>::Matrix()
{
	row = 0;
	col = 0;
	M = nullptr;
	cout << "construct Matrix" << '\n';
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
	cout << "construct Matrix with rows " << r << " and with columns " << c << '\n';
};

template <typename T>
Matrix<T>::Matrix<T>(const Matrix<T>& cpy)
{
	cout << "construct Matrix with rows " << cpy.row << " and with columns " << cpy.col << '\n';
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
int Matrix<T>::get_row() const
{
	return this->row;
}

template <typename T>
int Matrix<T>::get_col() const
{
	return this->col;
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
Matrix<T> Matrix<T>::operator*(const T right) const
{
	Matrix<T> c = *this;
	for (int i = 0; i < c.row; i++)
	{
		for (int j = 0; j < c.col; j++)
		{
			c.set_el(i, j, c.get_el(i, j) * right);
		}
	}	
	return c;
}

template <typename T>
Matrix<T>& Matrix<T>::Gauss()
{
	for (int i = 1; i < this->row; i++)
	{
		for (int j = 0; j < i; j++)
		{
			T coeff = -this->get_el(i, j) / this->get_el(j, j);
			for (int n = 0; n < this->col; n++)
			{
				T value = coeff * this->get_el(j, n) + this->get_el(i, n);
				this->set_el(i, n, value);
			}
		}
	}
	return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::Tran()
{
	Matrix<T> c(this->col, this->row);
	for (int i = 0; i < this->row; i++)
	{
		for (int j = 0; j < this->col; j++)
		{
			c.set_el(j, i, this->get_el(i, j));
		}
	}
	*this = c;
	return *this;
}

template <typename T>
T Matrix<T>::det() const
{
	if (this->row != this->col)
	{
		cout << "This is not square matrix, determinant can't be calculated" << '\n';
		return 0;
	}
	else
	{
		Matrix<T> c = *this;
		c.Gauss();
		T determinant = 1;
		for (int i = 0; i < c.row; i++)
		{
			determinant *= c.get_el(i, i);
		}
		return determinant;
	}
	
}

template <typename T>
Matrix<T> Matrix<T>::operator^(int q) const
{
	if (this->det() == 0)
	{
		cout << "Matrix has determinant - 0, there is no opposite matrix" << '\n';
		Matrix<T> d;
		return d;
	}
	else
	{
		Matrix<T> c(this->row, this->col * 2);
		for (int i = 0; i < c.row; i++)
		{
			for (int j = 0; j < c.col; j++)
			{
				if (j < this->col)
				{
					c.set_el(i, j, this->get_el(i, j));
				}
				else
				{
					if (j == i + this->col)
					{
						c.set_el(i, j, 1);
					}
				}
			}
		}
		for (int j = 0; j < this->col; j++)
		{
			T div = c.get_el(j, j);
			for (int k = 0; k < c.col; k++)
			{
				c.set_el(j, k, c.get_el(j, k) / div);
			}
			for (int i = 0; i < this->row; i++)
			{
				if (i != j)
				{
					T coeff = -c.get_el(i, j);
					for (int n = 0; n < c.col; n++)
					{
						T value = coeff * c.get_el(j, n) + c.get_el(i, n);
						c.set_el(i, n, value);
					}
				}
			}
		}
		Matrix<T> d(this->row, this->col);
		for (int i = 0; i < this->row; i++)
		{
			for (int j = 0; j < this->col; j++)
			{
				d.set_el(i, j, c.get_el(i, j + this->col));
			}
		}
		return d;
	}
}

template <typename T>
Matrix<T>::~Matrix()
{
	for (int i = 0; i < row; i++)
		delete[] M[i];
	delete[] M;
	cout << "destruct Matrix" << '\n';
};