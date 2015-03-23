extern func putchar(char putcharCharacter) -> int;

extern func puts(string str) -> int;

extern func sprintf(char^ str, string format, int i) -> int;

func itoa(int val, char^ str) -> char^ {
  string format = "%bd"
  char c 
  sprintf(str, "%d", val)
  str
}
