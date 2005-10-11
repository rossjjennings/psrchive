//-*-C++-*-

#ifndef _utils_psrfft_MKL_Transform_h_
#define _utils_psrfft_MKL_Transform_h_

#include "FTransform.h"

namespace FTransform {

  class MKL_Plan : public Plan {

  public:

    //! Constructor
    MKL_Plan (unsigned nfft, const std::string& _fft_call);

    //! Destructor
    ~MKL_Plan ();

    static int fcc1d (unsigned nfft, float* dest, const float* src);
    static int bcc1d (unsigned nfft, float* dest, const float* src);
    static int frc1d (unsigned nfft, float* dest, const float* src);
    static int bcr1d (unsigned nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<MKL_Plan> {
    public:
      Agent () : PlanAgent<MKL_Plan> ("MKL", nfft) { }
      MKL_Plan* new_plan (unsigned nfft, const std::string& call);
    };

  protected:

    float* mkl_plan;

    static Agent my_agent;
  };


}

#endif

