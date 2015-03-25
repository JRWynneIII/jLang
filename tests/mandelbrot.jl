import stdio;

func printDensity(int d) -> int {
  if d == 1 {
    putchar('0')
  }
  else {
    putchar('*')
  }
  1
}

func main() -> int
{
  int a
  int b
  double x
  double y
  int maxiter = 31
  int escapeval = 0
  double sq = 0.0
  int iter
  for b=0,32
  {
    for a = 0, 85
    {
      double zi = 0.0
      double zr = 0.0
      double zni = 0.0
      double znr = 0.0
      x = (a - 50.0) / 20.0
      y = (b - 16.0) / 10.0
      iter = 1
      zi = 0.0
      zr = 0.0
      for iter = 0, maxiter
      {
        zni = 2.0* zi * zr + y
        znr = zr * zr - zi* zi + x
        zi = zni
        zr = znr
        sq = zi*zi + zr*zr
        if sq > 4.0
        {
          escapeval = 1
        }
      }
      if escapeval == 1
      {
        putchar('*')
      }
      else
      {
        putchar(' ')
      }
      escapeval = 0
    }
    putchar('\n')
  }
  1
}
