
#include "random.h"

#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>

#if _DEBUG
#include <iostream>
using namespace std;
#endif

static const double random_max = RAND_MAX;

void random_init ()
{
  struct timeval t;
  gettimeofday (&t, NULL);
  srandom (t.tv_usec);
}

double random_double ()
{
  double val = double(random()) / random_max;
#if _DEBUG
  static double vmin = 1000;
  vmin = std::min(vmin,val);
  static double vmax = -1000;
  vmax = std::max(vmax,val);
  cerr << "check " << vmin << " " << vmax << endl;
#endif
  return val;
}
 
