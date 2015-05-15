import stdio;
import kernel;

kernel vecAdd(int^ a, int^ b, int^ c, int n) <- 10
{
  int id = tid.x
  if id < n {
    c[id] = a[id] + b[id]
  }
}

func main() -> int
{
  int^ a
  int^ b
  int^ c
  int n = 100000
  
  int i
  for i=0,n
  {
    a[i] = sin(i) * sin(i)
    b[i] = cos(i) * cos(i)
  }

  vecAdd(a,b,c,n)
}
