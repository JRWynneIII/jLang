import stdio;

func main() -> int
{
  int^ arr = iallocate(5)
  arr[0] = 1
  arr[1] = 2
  arr[2] = 333
  arr[3] = 432
  arr[4] = 5
  int i
  for i=0,4
  {
    printf("%d ",@arr[i])
  }
  1
}
