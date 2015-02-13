module stdio;

func main() -> int {
  int a = 4
  int b = 0
  char c = 'a'
  char d = 'z'
  if !b {
    putchar(c)
  }
  else {
    putchar(d)
  }
  if !a {
    putchar(c)
  }
  else {
    putchar(d)
  }
  1
}
