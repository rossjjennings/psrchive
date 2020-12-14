#include <iostream>
#include <cmath>
#include <cfloat>

using namespace std;

int main (int argc, char** argv)
{
  const bool save_signed = true;

  // due to definition of offset, max_short should be the same when
  // using either signed or unsigned
  float max_short;

  if (save_signed)
    max_short = pow(2.0,15.0)-1.0;
  else
    max_short = pow(2.0,16.0)-1.0;

  float min = 0.0;
  float max = 1.0;

  float offset = 0;
  if (save_signed)
    offset = 0.5 * (max + min);
  else
    offset = min;

  float scale = 1.0;

  // Test for dynamic range
  if (fabs(min - max) > (100.0 * FLT_MIN))
    scale = (max - min) / max_short;
  else
    cerr << "no range in min=" << min << " max=" << max << endl;

  int16_t imin = int16_t ((min-offset) / scale);
  int16_t imax = int16_t ((max-offset) / scale);

  cerr << "imin=" << imin << " imax=" << imax << endl;

  int16_t the_min = 0x01;
  the_min <<= 15;
  int16_t the_max = ~the_min;

  cerr << "the_min=" << the_min << " the_max=" << the_max << endl;

  float umin = imin * scale + offset;
  float umax = imax * scale + offset;

  cerr << "diff min=" << min - umin << endl;
  cerr << "diff max=" << max - umax << endl;

  return 0;
}

