void merge(int a[20], int low, int high, int mid)
{
    int i, j, k, c[50];
    i = low;
    k = low;
    j = mid + 1;
    while (i <= mid && j <= high)
    {
        if (a[i] < a[j])
        {
            c[k] = a[i];
            k++;
            i++;
        }
        else
        {
            c[k] = a[j];
            k++;
            j++;
        }
    }
    while (i <= mid)
    {
        c[k] = a[i];
        k++;
        i++;
    }
    while (j <= high)
    {
        c[k] = a[j];
        k++;
        j++;
    }
    for (i = low; i < k; i++)
    {
        a[i] = c[i];
    }
}

void mergesort(int a[20], int low, int high)
{
    int mid;
    if (low < high)
    {
        mid=(low+high)/2;
        mergesort(a,low,mid);
        mergesort(a,mid+1,high);
        merge(a,low,high,mid);
    }
    else
    {
			;
		}
}

int main()
{
    int a[20], i;
    
    printf("initial array\n");
    for (i = 0; i < 20; i++)
    {
        a[i]=20-i;
        printf("%d ",a[i]);
    }
    mergesort(a, 0, 19);
    
    printf("\nsorted array\n");
    for (i = 0; i < 20; i++)
    {
        printf("%d ",a[i]);
    }
    printf("\n");
}
