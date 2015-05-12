import stdio;

extern func printf(string fmt, int a) -> int;

func main() -> int
{
  int[10] arr
  arr[0] = 0
  arr[1] = 1
  arr[2] = 2
  arr[3] = 9988
  arr[4] = 4
  arr[5] = 5
  arr[6] = 6
  arr[7] = 7
  arr[8] = 8
  arr[9] = 9
  int i = 0
  for i=0,9
  {
    printf("%d ",@arr[i])
  }
  1
}

