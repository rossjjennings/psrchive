//-*-C++-*-

#ifndef _utils_psrfft_FFTW3_Transform_h_
#define _utils_psrfft_FFTW3_Transform_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW3

#include <fftw3.h>

namespace FTransform {
  class FFTW3_Plan;
}

#include "FTransform.h"

namespace FTransform {
  int fftw3_initialise();

  int fftw3_frc1d(unsigned ndat, float* dest, float* src);
  int fftw3_fcc1d(unsigned ndat, float* dest, float* src);
  int fftw3_bcc1d(unsigned ndat, float* dest, float* src);
  int fftw3_bcr1d(unsigned ndat, float* dest, float* src);

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
