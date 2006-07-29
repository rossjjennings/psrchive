//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _utils_psrfft_FFTW_Transform_h_
#define _utils_psrfft_FFTW_Transform_h_

#include "FTransform.h"

namespace FTransform {

  class FFTW_Plan : public Plan {

  public:

    //! Constructor
    FFTW_Plan (size_t nfft, const std::string& call);

    //! Destructor
    ~FFTW_Plan ();

    static int fcc1d (size_t nfft, float* dest, const float* src);
    static int bcc1d (size_t nfft, float* dest, const float* src);
    static int frc1d (size_t nfft, float* dest, const float* src);
    static int bcr1d (size_t nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<FFTW_Plan> {
    public:
      Agent () : PlanAgent<FFTW_Plan> ("FFTW", nfft) { }
      FFTW_Plan* new_plan (size_t nfft, const std::string& call);
    };

  protected:

    void* plan;
    float* tmp;

  };

  class FFTW_Plan2 : public Plan2 {

  public:
    FFTW_Plan2 ();
    FFTW_Plan2 (size_t nx, size_t ny, const std::string& call);
    ~FFTW_Plan2 ();
    void init (size_t nx, size_t ny, const std::string& call);

    static void
    fcc2d (size_t nx, size_t ny, float* dest, const float* src);
    static void
    bcc2d (size_t nx, size_t ny, float* dest, const float* src);

    class Agent : public PlanAgent2<FFTW_Plan2> {
    public:
      Agent () : PlanAgent2<FFTW_Plan2> ("FFTW", nfft) { }
      FFTW_Plan2* new_plan (size_t nx, size_t ny, const std::string& call);
    };

  protected:
    void* plan;


  };

}

#endif
