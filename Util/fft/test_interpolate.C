#include "interpolate.h"

int main () try {

  std::vector< std::complex<float> > C_in (128);
  std::vector< std::complex<float> > C_out (256);

  fft::interpolate (C_out, C_in);

  std::vector< double > R_in (128);
  std::vector< double > R_out (256);

  fft::interpolate (R_out, R_in);

  return 0;

}
catch (Error& error) {
  std::cerr << error << std::endl;
  return -1;
}
