//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FTransformPlan_h_
#define __FTransformPlan_h_

#include "FTransform.h"

namespace FTransform {

  //! Base class of one-dimensional Fast Fourier Transforms
  class Plan : public Reference::Able {
  public:

    Plan();
    virtual ~Plan ();

    //! Forward real-to-complex FFT 
    virtual void frc1d (size_t nfft, float* into, const float* from) = 0;

    //! Backward complex-to-real FFT 
    virtual void bcr1d (size_t nfft, float* into, const float* from) = 0;
    
    //! Forward complex-to-complex FFT
    virtual void fcc1d (size_t nfft, float* into, const float* from) = 0;

    //! Backward complex-to-complex FFT
    virtual void bcc1d (size_t nfft, float* into, const float* from) = 0;

    bool optimized;
    type call;
    size_t nfft;

  };

  //! Base class of two-dimensional Fast Fourier Transforms
  class Plan2 : public Reference::Able {
  public:

    virtual ~Plan2 () { }

    //! Forward complex-to-complex FFT
    virtual void fcc2d (size_t x, size_t y, float* into, const float* from)= 0;

    //! Backward complex-to-complex FFT
    virtual void bcc2d (size_t x, size_t y, float* into, const float* from)= 0;

    bool optimized;
    type call;
    size_t nx;
    size_t ny;

  };


}

#endif
