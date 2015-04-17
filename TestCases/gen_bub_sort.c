void bubble_sort_generic(float arr[10],int size)
{
	int i,j;
	float tmp;
	for(i = 0; i < size - 1; i++)
	{
		for(j = size - 2; j >= i; j = j-1)
		{
			if(arr[j+1]< arr[j]) // if <, then ascending
			{
				tmp = arr[j+1];
				arr[j+1] = arr[j];
				arr[j] = tmp;
			}
			else
			{
				;
			}
		}
	}
}


void print_arr(float arr[10], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		printf("%f ", arr[i]);
	}
	printf("\n");
}

int main()
{
	float arr[10];
	int size;
	size = 10;
	arr[0] = 1.32;
	arr[1] = 3;
	arr[2] = 2.11;
	arr[3] = 21;
	arr[4] = 1.11;
	arr[5] = 0.123;
	arr[6] = -125.51;
	arr[7] = 0.2222;
	arr[8] = 1 + 33.23123;
	arr[9] = 2;

	printf("The original values are:\n");
	print_arr(arr, size);
	printf("The sorted values are:\n");
	bubble_sort_generic(arr, size);
	print_arr(arr, size);
	printf("The sorted values are:\n");
	bubble_sort_generic(arr, size);
	print_arr(arr,size);

}
