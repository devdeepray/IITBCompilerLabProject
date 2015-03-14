int factorial(int x)
{
	int i;
	int result;
	result = 1;
	for(i = 0; i < x; i++)
	{
		result = result * i;
	}
	return result;
}

int factorial(int x)
{
	return x*x;
}

int main()
{
	int x[3];

	x[0] = factorial("s");
	x[1] = factorial(3);
	
}
