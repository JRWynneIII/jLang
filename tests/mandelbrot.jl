import stdio;

func printDensity(int d) -> int {
  char c
  char^ ch = c^
  if d == 1 {
    itoa(32,ch,10)
    putchar(c)
  }
  else {
    itoa(42,ch,10)
    putchar(c)
  }
  1
}

func main() -> int {
  int maxiter = 2000
  int xmin=-2
  int xmax=1
  int ymin=-2
  int ymax=2

  double threshold = 1.0
  double dist = 0.0
  int ix
  int iy
  double cx
  double cy
  int iter
  int i = 0
  double temp = 0.0
  double pixelWidth = 0.00375
  double pixelHeight = 0.005
  xmax=0
  ymax=1
  int x
  int y
  int tmp
  int xsquare
  int ysquare
  double escaperadius=2.0
  double ersquare = escaperadius*escaperadius
  for iy=0,ymax {
    tmp = ymin+iy
    cy = tmp*pixelHeight
    for ix = 0,xmax {
      tmp = xmin+ix
      cx=tmp*pixelWidth
      x=0
      y=0
      xsquare=x*x
      ysquare=y*y
      for iter=0,maxiter {
        if (xsquare+ysquare)<ersquare {
          y = 2.0*x*y+cy
          x=xsquare-ysquare+cx
          xsquare=x*x
          ysquare=y*y
        }
      }
      if iter == maxiter {
        printdensity(1)       
      }
      else {
        printdensity(0)
      }
    }
  }
  1
}
