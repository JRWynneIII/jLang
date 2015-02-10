module stdio;

extern func putchar(char c) -> int;

func incPtr(int^ a) -> int^ {
  int^ b = a + 1
  b
}
