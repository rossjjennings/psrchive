#include <iostream>
#include <cmath>
#include <cfloat>

using namespace std;

int main (int argc, char** argv)
{
  double val = 1.9999;

  float x = -val;
  int16_t i = x;
  cerr << "float x=" << x << " casts to int16_t i=" << i << endl;

  x = val;
  i = x;
  cerr << "float x=" << x << " casts to int16_t i=" << i << endl;

  x = -val;
  i = round(x);
  cerr << "float x=" << x << " rounds to int16_t i=" << i << endl;

  x = val;
  i = round(x);
  cerr << "float x=" << x << " rounds to int16_t i=" << i << endl;

  double the_min = 1-pow(2,15);
  double the_max = pow(2,15)-2;
  cerr << "signed: the_min=" << the_min << " the_max=" << the_max << endl;
 
  float min = 0.0;
  float max = 1.0;

  float offset = (min*the_max -max*the_min) / (the_max - the_min);

  float scale = 1.0;

  // Test for dynamic range
  if (fabs(min - max) > (100.0 * FLT_MIN))
    scale = (max - min) / (the_max - the_min);
  else
    cerr << "no range in min=" << min << " max=" << max << endl;

  cerr << "scale=" << scale << " offset=" << offset << endl;

  int16_t imin = int16_t ((min-offset) / scale);
  int16_t imax = int16_t ((max-offset) / scale);

  cerr << "imin=" << imin << " imax=" << imax << endl;

  float umin = imin * scale + offset;
  float umax = imax * scale + offset;

  cerr << "umin=" << umin << " umax=" << umax << endl;

  cerr << "diff min=" << min - umin << endl;
  cerr << "diff max=" << max - umax << endl;

  return 0;
}

