/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarAtanc.h"

#include <iostream>
using namespace std;

int main (int argc, char** argv)
{
  for (double x=-0.5; x<=0.5; x+=0.001)
  {
    if (x == 0.0)
      continue;

    double d1 = 0;
    double y1 = atanc (x, &d1);

    double z = x * M_PI * 0.5;
    double y2 = atan (z) / z;
    double d2 = 0.5*M_PI * (1.0 / (z*(1+z*z)) - y2 / z);

    cerr << " y1=" << y1 << " d1=" << d1
	 << " y2=" << y2 << " d2=" << d2 << endl;

    if ( fabs(y1-y2) > 1e-15 )
    {
      cerr << " atanc(" << x << ")=" << y1 
	   << " != atan(" << z << ")/" << z << "=" << y2 << endl;
      return -1;
    }

    if ( fabs(d1-d2) > 1e-12 )
    {
      cerr << " datanc(" << x << ")/dx=" << d1 
	   << " != expected=" << d2 
	   << " (diff=" << fabs(d1-d2) << ")" << endl;
      return -1;
    }
  }

  for (double x=-10; x <= 10; x+=0.1)
    cout << x << " " << atanc(x) << endl;

  return 0;
}
