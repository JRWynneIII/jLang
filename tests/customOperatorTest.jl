module stdio;

extern func putchar(char c) -> int;

func unary!(int a) -> int {
  int ret = 0
  
  if a > 0 {
    ret = 0
  }
  else {
    ret = 1
  }
  ret
}

func main() -> int {
  int d = 1
  char c = 'a'
  char b = 'z'
  if !d {
    putchar(c)
  }
  else {
    putchar(b)
  }
  1
}
