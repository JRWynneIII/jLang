module stdio;

#This test should fail! Have to catch it before llvm gets it though

extern func putchar(char b) -> int;

func writez() -> int {
  char d = 'z'
  putchar(d)
}

func write(char c) -> int {
  putchar(d)
  1
}

func main() -> int {
  char a = 'q'
  write(a)
  writez()
  1
}

