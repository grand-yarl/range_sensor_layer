// ConsoleApplication9.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include "Matrix.h"
#include "Node.h"
#include "graph.h"


using namespace std;



int main()
{

	Matrix <double> b;
	cin >> b;
	cout << b;
	
	graph <double> c(b);
	cout << c.bfs(0) << '\n';

	system("pause");
	return 0;
}

