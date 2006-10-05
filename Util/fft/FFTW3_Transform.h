//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _utils_psrfft_FFTW3_Transform_h_
#define _utils_psrfft_FFTW3_Transform_h_

#include "FTransform.h"

#include <fftw3.h>

namespace FTransform {

  class FFTW3_Plan : public Plan {

  public:

    //! Constructor
    FFTW3_Plan (size_t nfft, const std::string& call);

    //! Destructor
    ~FFTW3_Plan ();

    static int fcc1d (size_t nfft, float* dest, const float* src);
    static int bcc1d (size_t nfft, float* dest, const float* src);
    static int frc1d (size_t nfft, float* dest, const float* src);
    static int bcr1d (size_t nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<FFTW3_Plan> {
    public:
      Agent () : PlanAgent<FFTW3_Plan> ("FFTW3", unnormalized) { }
      FFTW3_Plan* new_plan (size_t nfft, const std::string& call);
    };

  protected:

    void* plan;

  };


  class FFTW3_Plan2 : public Plan2 {

  public:

    //! Constructor
    FFTW3_Plan2 (size_t nx, size_t ny, const std::string& call);

    //! Destructor
    ~FFTW3_Plan2 ();

    static void fcc2d (size_t nx, size_t ny, float* , const float* );
    static void bcc2d (size_t nx, size_t ny, float* , const float* );

    class Agent : public PlanAgent2<FFTW3_Plan2> {
    public:
      Agent () : PlanAgent2<FFTW3_Plan2> ("FFTW3", unnormalized) { }

      FFTW3_Plan2*
      new_plan (size_t nx, size_t ny, const std::string& call);
    };

  protected:

    void* plan;


  };

}

#endif
