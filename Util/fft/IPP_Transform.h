//-*-C++-*-

#ifndef _utils_psrfft_IPP_Transform_h_
#define _utils_psrfft_IPP_Transform_h_

#if HAVE_IPP

#include "Transform.h"

#include <ipps.h>

namespace Transform {
  int ipp_initialise();

  int ipp_frc1d(unsigned ndat, float* dest, float* src);
  int ipp_fcc1d(unsigned ndat, float* dest, float* src);
  int ipp_bcc1d(unsigned ndat, float* dest, float* src);

  class IPP_Plan : public Plan {
  public:
    IPP_Plan();
    IPP_Plan(unsigned _ndat, unsigned _ilib, string _fft_call);
    ~IPP_Plan();
    void init(unsigned _ndat, unsigned _ilib, string _fft_call);

    Ipp8u* pBuffer;
    void* Spec;
  };
}

#endif

#endif
