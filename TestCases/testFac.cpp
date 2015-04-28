int fac(int n)
{
  if(n==0)
  {
   return 1;
  }
  else
  {
    ;
  }
   
  return n*fac(n-1);
}

int main()
{
  int x;
  x = 10;
  printf("Fac of x: %d", fac(x));
  return 23;
}
