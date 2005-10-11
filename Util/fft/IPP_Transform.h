//-*-C++-*-

#ifndef _IPP_Transform_h_
#define _IPP_Transform_h_

#include "FTransform.h"
#include <ipps.h>

namespace FTransform {

  class IPP_Plan : public Plan {

  public:

    //! Constructor
    IPP_Plan (unsigned nfft, const std::string& _fft_call);

    //! Destructor
    ~IPP_Plan ();

    static int fcc1d (unsigned nfft, float* dest, const float* src);
    static int bcc1d (unsigned nfft, float* dest, const float* src);
    static int frc1d (unsigned nfft, float* dest, const float* src);
    static int bcr1d (unsigned nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<IPP_Plan> {
    public:
      Agent () : PlanAgent<IPP_Plan> ("IPP", nfft) { }
      IPP_Plan* new_plan (unsigned nfft, const std::string& call);
    };

  protected:

    static Agent my_agent;

    Ipp8u* pBuffer;
    void* Spec;

  };
}

#endif

