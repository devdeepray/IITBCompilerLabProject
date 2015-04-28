int fib_helper(int n, int arr[30])
{
//printf("arr addr %d", arr);
//printf("\narrval %d\n", arr[n]);
 //return 0;
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
  int f[5][30];
 int i;
  for(i = 2; i <= n; i++)
  {
   f[2][i] = -1;
//    printf("%d %d\n", i, f[i]);
  }
  // 0th and 1st number of the sries are 0 and 1
  f[2][0] = 1;
  f[2][1] = 1;
//printf("f addr %d", f);
  return fib_helper(n, f[2]);
}

int main()
{
  int n;
  for(n = 0; n < 20; n++)
  {
    printf("fib(%d) is %d\n",n, fib(n));
  }
  return 0;
}
