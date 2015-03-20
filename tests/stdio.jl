extern func putchar(char putcharCharacter) -> int;

#extern func itoa(int val, char^ str, int base) -> char^;

extern func sprintf(char^ str, string format, int i) -> int;

func itoa(int val, char^ str, int base) -> char^ {
  string format = "%d"
  sprintf(str, format, val)
  str
}

func binary|(int lhs, int rhs) -> int {
  int ret = 0
  if lhs > 0 {
    ret = 1
  }
  else {
    ret = 0
  }
  if rhs > 0 {
    ret = 1
  }
  else {
    ret = 0
  }
  ret
}

func binary&(int lhs, int rhs) -> int {
  int ret = 0
  if !lhs {
    ret = 0
  }
  else {
    ret = rhs
  }
  ret
}

