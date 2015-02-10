#include <stdio.h>
extern int* incPtr(int* a);

int main()
{
  int a[3] = { 33, 23, 19 };
  int* b = incPtr(a);
  printf("%d\n",*b);
}
