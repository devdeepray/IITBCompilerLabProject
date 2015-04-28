float f(float x)
{
    return x*x + 5*x + 1;
}
float df (float x)
{
    return  2*x + 5;
}
float fabs(float x)
{
    if(x>0) 
	{
		return x;
	}
	else
	{
		return -x;
	}
}
int main()
{
    int itr, maxmitr;
    float h, x0, x1, allerr;
    x0 = 100;
    allerr = 0.001;
    maxmitr = 10000;
    for (itr=1; itr<=maxmitr; itr++)
    {
        h=f(x0)/df(x0);
        x1=x0-h;
        printf(" At Iteration no. %d, x = %f\n", itr, x1);
        if (fabs(h) < allerr)
        {
            printf("After %d iterations, root = %f\n", itr, x1);
            return 0;
        }
        else
	{
		;
	}
        x0=x1;
    }
    printf(" The required solution does not converge or iterations are insufficient\n");
    return 1;
}
