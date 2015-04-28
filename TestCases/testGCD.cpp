int gcd( int m, int n)
{ 
  //return gcd(m,n);
  if(m == n)
  {
    return m;
  }
  else
  {
   ;
  }
  if( m > n)
  {
    return gcd(m-n,n);
  }
  else
  {
    return gcd(m,n-m);
  }
}

int main()
{
  int m,n;
  m = 7;
  n = 53;

  printf("GCD of m and n %d\n",gcd(m,n));
  return 0;
}
