//-*-C++-*-

#ifndef _utils_psrfft_FFTW3_Transform_h_
#define _utils_psrfft_FFTW3_Transform_h_

#if HAVE_FFTW3

#include <fftw3.h>

#include "Transform.h"

namespace Transform {
  int fftw3_initialise();

  int fftw3_frc1d(unsigned ndat, float* dest, float* src);
  int fftw3_fcc1d(unsigned ndat, float* dest, float* src);
  int fftw3_bcc1d(unsigned ndat, float* dest, float* src);

  class FFTW3_Plan : public Plan {
  public:
    FFTW3_Plan();
    FFTW3_Plan(unsigned _ndat, unsigned _ilib, string _fft_call);
    ~FFTW3_Plan();
    void init(unsigned _ndat, unsigned _ilib, string _fft_call);

    fftwf_plan* plan;
  };

}

#endif

#endif
