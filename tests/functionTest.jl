module stdio;

extern func putchar(char b) -> int;

func write(char c) -> int {
  putchar(c)
  1
}

func writez() -> int {
  char d = 'z'
  putchar(d)
}

func main() -> int {
  char a = 'q'
  write(a)
  writez()
  1
}

