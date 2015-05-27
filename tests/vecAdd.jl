import stdio;

func main() -> int
{
  int n = 100000
  double^ a = dallocate(n)
  double^ b = dallocate(n)
  double^ c = dallocate(n)
  int i
  int val = n-1
  for i=0,val {
    a[i] = sin(i) * sin(i)
    b[i] = cos(i) * cos(i)
  }
  for i=0,val {
    c[i] = @a[i] + @b[i]
  }
  double sum = 0.0
  for i=0,val {
    sum = sum + c[i]
  }

  sum = sum/100000.0

  printf("Final result: %f ",sum)
  1
}
