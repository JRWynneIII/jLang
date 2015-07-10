import stdio;

class test
{
  int a
  double b
  char c
}

func main() -> int
{
  test obj
  obj.a = 10
  obj.b = 19.3
  obj.c = 's'
  char^ d = obj.c
  putchar(@d)
  1
}
