#include "interpolate.h"

int main () try {

  vector< complex<float> > C_in (100);
  vector< complex<float> > C_out (200);

  fft::interpolate (C_out, C_in);

  vector< double > R_in (100);
  vector< double > R_out (200);

  fft::interpolate (R_out, R_in);

  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
