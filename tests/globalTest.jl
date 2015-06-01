import stdio;

global int globval;

func set() -> int
{
  globval = 1009
  1
}

func main() -> int
{
  set()
  printf("%d", globval)
  putchar('\n')
  1
}

