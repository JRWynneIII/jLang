module stdio;

extern func putchar(char b) -> int;

func main() -> int {
  int i = 0
  int j = 0
  char a = 'z'
  for i=0,10 {
    a = 'a'
    putchar(a)
  }
  for i=0,10 {
    for j=0,10 {
      a = 'z'
      putchar(a)
    }
  }
  1
}

