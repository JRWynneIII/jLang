extern func putchar(char c) -> int;
extern func printf(string fmt,int a) -> int;

func main() -> int {
  int i = 0
  int j = 0
  int k = 0
  char a = 'z'
  char newLine = '\n'
  for i=0,10 {
    a = 'a'
    putchar(a)
  }
  putchar(newLine)
  for i=0,10 {
    for j=0,10 {
      a = 'z'
      k = k + 1
      putchar(a)
      putchar(' ')
      printf("%d",k)
    }
    putchar(newLine)
  }
  1
}

