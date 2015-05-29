import stdio;

global int globval;

func set() -> int
{
  globval = 1009
  printf("%d", globval)
  1
}

func main() -> int
{
  globval = 4
  printf("%d", globval)
  set()
  1
}

