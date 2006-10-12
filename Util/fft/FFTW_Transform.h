//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _Utils_fft_FFTW_Transform_h_
#define _Utils_fft_FFTW_Transform_h_

#include "FTransformAgent.h"

namespace FTransform {

  class FFTW {

  public:

    class Plan : public FTransform::Plan {

    public:

      Plan (size_t nfft, type t);
      ~Plan ();

      void fcc1d (size_t nfft, float* dest, const float* src);
      void bcc1d (size_t nfft, float* dest, const float* src);
      void frc1d (size_t nfft, float* dest, const float* src);
      void bcr1d (size_t nfft, float* dest, const float* src);
      
    protected:
      
      void* plan;
      float* tmp;

    };

    class Plan2 : public FTransform::Plan2 {

    public:

      Plan2 (size_t nx, size_t ny, type t);
      ~Plan2 ();

      void fcc2d (size_t nx, size_t ny, float* dest, const float* src);
      void bcc2d (size_t nx, size_t ny, float* dest, const float* src);

    protected:

      void* plan;

    };

    class Agent : public PlanAgent<FFTW> {
    public:
      Agent () : PlanAgent<FFTW> ("FFTW", unnormalized) { }
    };

  };

}

#endif
