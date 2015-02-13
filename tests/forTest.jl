extern func putchar(char c) -> int;

func main() -> int {
  int i = 0
  int j = 0
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
      putchar(a)
    }
    putchar(newLine)
  }
  1
}

