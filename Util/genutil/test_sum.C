/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "templates.h"
#include <iostream>
#include <math.h>

using namespace std;

int main ()
{
  vector<float> x (5);

  x[0] = 1.2;
  x[1] = 3.4;
  x[2] = 5.6;
  x[3] = 7.8;
  x[4] = 9.0;

  double dsum = sum<double>(x.begin(),x.end());
  float fsum = sum(x);

  double expect = 27.0;

  if (fabs(fsum - dsum) > 1e-6 || fabs(dsum - expect) > 1e-6) {
    cerr << "sum template error" << endl;
    cerr << "fsum=" << fsum << " dsum=" << dsum << " expect=" << expect
         << " diff=" << dsum-expect << endl;
    return -1;
  }

  cerr << "sum template passes all tests" << endl;
  return 0;
}
