#include "interpolate.h"

int main () try {

  vector< complex<float> > C_in (128);
  vector< complex<float> > C_out (256);

  fft::interpolate (C_out, C_in);

  vector< double > R_in (128);
  vector< double > R_out (256);

  fft::interpolate (R_out, R_in);

  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
