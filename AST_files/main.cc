#include <iostream>
#include "ASTNodes.h"
int main()
{
	FloatConst fc1(2.33), fc2(3.22);
	Ass as1(&fc1, &fc2);
	as1.print();
}
