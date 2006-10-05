//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _IPP_Transform_h_
#define _IPP_Transform_h_

#include "FTransform.h"
#include <ipps.h>

namespace FTransform {

  class IPP_Plan : public Plan {

  public:

    //! Constructor
    IPP_Plan (size_t nfft, const std::string& _fft_call);

    //! Destructor
    ~IPP_Plan ();

    static int fcc1d (size_t nfft, float* dest, const float* src);
    static int bcc1d (size_t nfft, float* dest, const float* src);
    static int frc1d (size_t nfft, float* dest, const float* src);
    static int bcr1d (size_t nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<IPP_Plan> {
    public:
      Agent () : PlanAgent<IPP_Plan> ("IPP", unnormalized) { }
      IPP_Plan* new_plan (size_t nfft, const std::string& call);
    };

  protected:

    Ipp8u* pBuffer;
    void* Spec;

  };
}

#endif

