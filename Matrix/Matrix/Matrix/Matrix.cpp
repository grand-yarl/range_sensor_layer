// ConsoleApplication9.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include "Matrix.h"

using namespace std;




int main()
{
	Matrix <int> a;
	Matrix <double> b(3, 3);
	b.set_el(0, 0, 1);
	b.set_el(0, 1, 2);
	b.set_el(0, 2, 3);
	cout << b;
	Matrix <double> c(3, 3);
	c.set_el(0, 0, 1);
	c.set_el(1, 0, 2);
	c.set_el(2, 0, 3);
	cout << c;
	Matrix <double> d = c * b;
	cout << d;

	system("pause");
	return 0;
}

