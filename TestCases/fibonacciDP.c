int fib_helper(int n, int arr[100])
{
  if(arr[n] != -1)
  {
    return arr[n];
  }
  else
  {
    arr[n] = fib_helper(n-1, arr) + fib_helper(n-2, arr);
    return arr[n];
  }
}

int fib(int n)
{
  // Declare an array to store fibonacci numbers. 
  int f[100];
  int i;
  for(i = 2; i <= n; i++)
  {
    f[i] = -1;
  }
  // 0th and 1st number of the series are 0 and 1
  f[0] = 0;
  f[1] = 1;

  return f[n];
}

int main()
{
  int n;
  n = 9;
  printf("%d", fib(n));
  return 0;
}
