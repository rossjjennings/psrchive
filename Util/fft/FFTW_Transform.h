//-*-C++-*-

#ifndef _utils_psrfft_FFTW_Transform_h_
#define _utils_psrfft_FFTW_Transform_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW

#include <rfftw.h>

namespace FTransform {
  class FFTW_Plan;
}

#include "FTransform.h"

namespace FTransform {
  int fftw_initialise();

  int fftw_frc1d(unsigned ndat, float* dest, float* src);
  int fftw_fcc1d(unsigned ndat, float* dest, float* src);
  int fftw_bcc1d(unsigned ndat, float* dest, float* src);

  class FFTW_Plan : public Plan {
  public:
    FFTW_Plan();
    FFTW_Plan(unsigned _ndat, unsigned _ilib, string _fft_call);
    ~FFTW_Plan();
    void init(unsigned _ndat, unsigned _ilib, string _fft_call);
    
    void* plan;
    float* tmp;
  };
}

#endif

#endif
