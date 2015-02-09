module stdio;

extern func putchar(char b) -> int;

func main() -> int {
  char a = 'a'
  char b
  b = a
  putchar(b)
  1
}

