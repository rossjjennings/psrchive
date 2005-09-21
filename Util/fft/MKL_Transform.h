//-*-C++-*-

#ifndef _utils_psrfft_MKL_Transform_h_
#define _utils_psrfft_MKL_Transform_h_

#include "FTransform.h"
#include <vector>

namespace FTransform {

  int mkl_initialise();

  int mkl_frc1d(unsigned ndat, float* dest, float* src);
  int mkl_fcc1d(unsigned ndat, float* dest, float* src);
  int mkl_bcc1d(unsigned ndat, float* dest, float* src);
  int mkl_bcr1d(unsigned ndat, float* dest, float* src);

  class MKL_Plan : public Plan {
  public:
    MKL_Plan();
    ~MKL_Plan();
    MKL_Plan(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);
    void init(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);
    float* mkl_plan;
  };

}

#endif

