// ConsoleApplication9.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include "Matrix.h"
#include "Node.h"


using namespace std;




int main()
{
	/*Matrix <int> a;
	Matrix <double> b(3, 3);
	double n = 1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) 
		{
			b.set_el(i, j, n);
			n++;
		}
	}
	b.set_el(0, 0, 49);
	b.set_el(1, 2, 13);
	b.set_el(2, 1, 51);
	Matrix<double> c = (b ^ (-1));*/

	Node<int> s[10];
	for (int i = 0; i < 10; i++)
	{
		s[i].data = i; 
		cout << s[i].data << '\n';
	}
		
	que<int> q;
	for (int i = 0; i < 10; i++)
	{
		q.push(s[i]);
		cout << q.lenght() << '\n';
	}

	q.search(9);
	q.search(11);
	for (int i = 0; i < 11; i++)
	{
		Node<int> c = *q.get();
		cout << c.data << '\n';
		cout << q.lenght() << '\n';
	}


	system("pause");
	return 0;
}

