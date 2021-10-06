#pragma once
#include "Node.h"
#include "Matrix.h"
using namespace std;

template <typename T>
class graph
{
	private:
		int* id;
		Matrix<double> M;
	public:
		graph(Matrix<double>& m);
		que<int> bfs(int start);

		~graph();
};


template <typename T>
graph<T>::graph(Matrix<double>& m)
{
	if (m.get_row() == m.get_col())
	{
		int* id = new int[m.get_row()];
		for (int i = 1; i < m.get_row() + 1; i++)
		{
			id[i - 1] = i;
			cout << id[i - 1] << '\n';
		}
		this->M = m;
		cout << "construct graph" << '\n';
	}
	else
		cout << "Matrix has invalid size" << '\n';
}

template <typename T>
que<int> graph<T>::bfs(int start)
{
	Node<int>* n = new Node<int>[M.get_row()];
	int* status = new int[M.get_row()];
	for (int i = 0; i < M.get_row(); i++)
	{
		n[i].new_data(i);
		status[i] = 0;
	}
	que<int> out;
	que<int> temp(n[start]);
	status[start] = 1;
	while (temp.lenght() > 0)
	{
		Node<int>* current = temp.get();
		status[current->get_data()] = 2;
		out.push(*current);
		for (int j = 0; j < M.get_row(); j++)
		{
			if ((M.get_el(current->get_data(), j) > 0) && (status[j] == 0))
			{
				status[j] = 1;
				temp.push(n[j]);
			}
		}
	}
	cout << "end bfs" << '\n';
	return out;
}



template <typename T>
graph<T>::~graph()
{
	delete[] id;
	cout << "destruct graph" << '\n';
}