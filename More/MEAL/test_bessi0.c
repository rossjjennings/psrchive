#include <stdio.h>

extern double bessi0 (double);

int main ()
{
  double x = 0;
  for (x=0; x<10; x+=0.1)
    printf ("%lf %lf \n", x, bessi0(x));
  return 0;
}
