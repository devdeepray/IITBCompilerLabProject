void main()
{
  int list[5];
  int i;
  

 for(i = 0; i < 5;i++)
  { 
    list[i]=i;
    list[i]++; 
  }

  for( i = 0; i < 5 ; i++)
  {
    printf("%d\n",list[i]);
  }

}
