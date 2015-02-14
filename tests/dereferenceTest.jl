import stdio;

func main() -> int {
  char a = 'a'
  char^ b = a^
  char c = 'q'
  c = @b
  putchar(c)
  1
}
