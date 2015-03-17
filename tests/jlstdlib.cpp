#include<stdio.h>
#include <stdlib.h>

extern "C" char* itoa(int i, char* s, int dummy_radix) 
{
  sprintf(s, "%d", i);
  return s;
}
