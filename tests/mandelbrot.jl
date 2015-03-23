import stdio;

func printDensity(int d) -> int {
  char c
  char^ ch = c^
  if d == 1 {
    putchar('.')
  }
  else {
    putchar('*')
  }
  1
}

func main() -> int
{
  double xsize = 59.0
  double ysize = 21.0
  double minre = -2.0
  double minim = -1.0
  double maxre = 1.0
  double maxim = 1.0
  double stepx = 0.05084745762
  double stepy = 0.09523809523
  double y
  double x
  double zr
  double zi
  int n
  double a
  double b
  for y = 0.0, ysize, 1.0
  {
    double im = minim+stepy*y
    for x = 0.0, xsize
    {
      double re = minre+stepx*x
      zr = re
      zi = im
      for n=0, 30
      {
        a = zr*zr
        b = zi*zi 
        if (a+b)>4.0
        {
          printDensity(1)
        }
        else
        {
          zi = 2.0*zr*zi+im
          zr = a-b+re
          printDensity(0)
        }
      }
    }
  }
}
