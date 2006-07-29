//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _utils_psrfft_MKL_Transform_h_
#define _utils_psrfft_MKL_Transform_h_

#include "FTransform.h"

namespace FTransform {

  class MKL_Plan : public Plan {

  public:

    //! Constructor
    MKL_Plan (size_t nfft, const std::string& _fft_call);

    //! Destructor
    ~MKL_Plan ();

    static int fcc1d (size_t nfft, float* dest, const float* src);
    static int bcc1d (size_t nfft, float* dest, const float* src);
    static int frc1d (size_t nfft, float* dest, const float* src);
    static int bcr1d (size_t nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<MKL_Plan> {
    public:
      Agent ();
      virtual ~Agent();
      MKL_Plan* new_plan (size_t nfft, const std::string& call);
    };

  protected:

    float* mkl_plan;

  };


}

#endif

