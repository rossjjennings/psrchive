#include "normal.h"
#include <stdio.h>
#include <math.h>

int main ()
{
  double a = 1.6;
  
  if (normal_probability (a) <= normal_probability (a+1)) {
    fprintf (stderr, "probability is not decreasing\n");
    return -1;
  }

  if (fabs(normal_probability (a) - normal_probability (-a)) > 1e-12) {
    fprintf (stderr, "probability is not symmetric\n");
    fprintf (stderr, "a=%lf; prob(a)=%lf; prob(-a)=%lf\n", a,
	     normal_probability (a), normal_probability (-a));
    return -1;
  }

  if (normal_cummulative (a) >= normal_cummulative (a+1)) {
    fprintf (stderr, "cummulative is not increasing\n");
    return -1;
  }

  if (fabs(normal_cummulative (a) + normal_cummulative (-a) -1) > 1e-12) {
    fprintf (stderr, "cummulative is not anti-symmetric about 0.5\n");
    fprintf (stderr, "a=%lf; cummulative(a)=%lf; cummulative(-a)=%lf\n", a,
	     normal_cummulative (a), normal_cummulative (-a));
    return -1;
  }

  if (normal_cummulative (0) != 0.5) {
    fprintf (stderr, "cummulative(0) != 0.5\n");
    return -1;
  }

  fprintf (stderr, "all normal distribution functions pass tests\n");

  return 0;
}

