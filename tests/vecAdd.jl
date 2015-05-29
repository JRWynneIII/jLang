import stdio;

func main() -> int
{
  int n = 100000
  double^ a = dallocate(n)
  double^ b = dallocate(n)
  double^ c = dallocate(n)
  int i

  for i=0,n {
    a[i] = sin(i) * sin(i)
    b[i] = cos(i) * cos(i)
  }

  for i=0,n {
    c[i] = @a[i] + @b[i]
  }
  double sum = 0.0
  for i=0,n {
    sum = sum + @c[i]
  }

  sum = sum/n

  printd("Final result: %lf ",sum)
  1
}
