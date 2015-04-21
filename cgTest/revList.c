void main()
{ 
  int i,temp;
  int list[10];
 
  for( i = 0; i < 10 ;i++)
  {
    list[i] = i + 1;
  }

  for( i = 0; i < 5 ;i++)
  {  
   temp = list[9-i];
   list[9-i] = list[i];
   list[i] = temp;
  }
  
  for( i = 0 ; i < 10 ;i++)
  {
   printf("%d \n",list[i]);
  }
 

}
