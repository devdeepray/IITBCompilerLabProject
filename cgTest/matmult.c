void main()
{
    int a[10][10], b[10][10], mult[10][10], r1, c1, r2, c2, i, j, k,l;
    r1 = 10;
    c1 = 10;
    r2 = 10;
    c2 = 10;
    for(l = 0; l < 10000; l++)
    {
	for(i = 0; i < r1; i++)
	{
		for(j = 0; j < c1; j++)
		{
			a[i][j] = ((i+1) * (j + 1) * 41)  - (((i+1) * (j+1) * 41) / 17) * 17;
		}
	}
	
	for(i = 0; i<r2; i++)
	{
		for(j = 0; j < c2; j++)
		{
			b[i][j] = ((i+1) * (j+1) * 37)  - (((i+1) * (j+1) * 37) / 19) * 19;
		}
	}

	printf("\nMatrix A:\n");
    for(i=0; i<r1; i++)
    {
		for(j=0; j<c2; j++)
		{
			printf("%d  ",a[i][j]);
			if(j==c2-1)
			{
				printf("\n\n");
			}
			else
			{
				;
			}
		}
	}
	
	
	printf("\nMatrix B:\n");
  for(i=0; i<r1; i++)
  {
		for(j=0; j<c2; j++)
		{
			printf("%d  ",b[i][j]);
			if(j==c2-1)
			{
				printf("\n\n");
			}
			else
			{
				;
			}
		}
	}

    

    for(i=0; i<r1; i++)
    {
		for(j=0; j<c2; j++)
		{
			mult[i][j]=0;
		}
	}
//~ 
    for(i=0; i<r1; i++)
	{
		for(j=0; j<c2; j++)
		{
			for(k=0; k<c1; k++)
			{
				mult[i][j] = a[i][k]*b[k][j] + mult[i][j];
			}
		}
	}

	
	
	printf("\nOutput Matrix:\n");
  for(i=0; i<r1; i++)
  {
		for(j=0; j<c2; j++)
		{
			printf("%d  ",mult[i][j]);
			if(j==c2-1)
			{
				printf("\n\n");
				;
			}
			else
			{
				;
			}
		}
	}
}
}
