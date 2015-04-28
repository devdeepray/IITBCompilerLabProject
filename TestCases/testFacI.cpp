int main()
{
  int x,fac;
  x = 10;
  fac = 1;
 
  if(x < 0)
  {
    printf("Fac undefined for negative num\n");
    return 0;
  }
  else
  {
    ;
  }
 
  while(x>0)
  {  
    fac = fac*(x);
    x = x-1;     
  }

  printf("Fac of x: %d\n",fac);

}
