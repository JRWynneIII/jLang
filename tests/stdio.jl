extern func malloc(int a) -> double^;

func dallocate(int a) -> double^ {
  malloc(a)
}

extern func malloc(int a) -> int^;

func iallocate(int a) -> int^ {
  malloc(a)
}

extern func putchar(char putcharCharacter) -> int;
extern func printf(string fmt, double a) -> int;
extern func puts(string str) -> int;
extern func puts(char str) -> int;
extern func sprintf(char^ str, string format, int i) -> int;
extern func abs(int n) -> int;
extern func malloc(int a) -> double^;
extern func sin(int a) -> double;
extern func cos(int a) -> double;
