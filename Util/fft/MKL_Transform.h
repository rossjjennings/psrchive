//-*-C++-*-

#ifndef _utils_psrfft_MKL_Transform_h_
#define _utils_psrfft_MKL_Transform_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MKL

#include <vector>

namespace FTransform {
  class MKL_Plan;
}

#include "FTransform.h"

namespace FTransform {
  int mkl_initialise();

  int mkl_frc1d(unsigned ndat, float* dest, float* src);
  int mkl_fcc1d(unsigned ndat, float* dest, float* src);
  int mkl_bcc1d(unsigned ndat, float* dest, float* src);

  class MKL_Plan : public Plan {
  public:
    MKL_Plan();
    ~MKL_Plan();
    MKL_Plan(unsigned _ndat, unsigned _ilib, string _fft_call);
    void init(unsigned _ndat, unsigned _ilib, string _fft_call);
    float* mkl_plan;
  };

}

#endif

#endif
