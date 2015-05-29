#include<stdio.h>
#include<stdlib.h>
#include<math.h>

extern "C" int* iallocate(int ints)
{
  return (int*)malloc(ints*sizeof(int));
}

extern "C" double* dallocate(int doubles)
{
  return (double*)malloc(doubles*sizeof(double));
}

extern "C" int printd(char* fmt, double val)
{
  return printf(fmt,val);
}
