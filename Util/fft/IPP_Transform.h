//-*-C++-*-

#ifndef _IPP_Transform_h_
#define _IPP_Transform_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_IPP

#include <ipps.h>

namespace IPPTransform {
  class IPP_Plan;
}

#include "FTransform.h"

namespace FTransform {
  int ipp_initialise();

  int ipp_frc1d(unsigned ndat, float* dest, float* src);
  int ipp_fcc1d(unsigned ndat, float* dest, float* src);
  int ipp_bcc1d(unsigned ndat, float* dest, float* src);

  class IPP_Plan : public FTransform::Plan {
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
