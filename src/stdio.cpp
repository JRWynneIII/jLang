#include<stdio.h>
#include<stdlib.h>

extern "C" int* iallocate(int ints)
{
  return (int*)malloc(ints*sizeof(int));
}

extern "C" double* dallocate(int doubles)
{
  return (double*)malloc(doubles*sizeof(double));
}
