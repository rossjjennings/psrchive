//-*-C++-*-

#ifndef _utils_psrfft_FFTW_Transform_h_
#define _utils_psrfft_FFTW_Transform_h_

#include "FTransform.h"

namespace FTransform {

  class FFTW_Plan : public Plan {

  public:

    //! Constructor
    FFTW_Plan (unsigned nfft, const std::string& call);

    //! Destructor
    ~FFTW_Plan ();

    static int fcc1d (unsigned nfft, float* dest, const float* src);
    static int bcc1d (unsigned nfft, float* dest, const float* src);
    static int frc1d (unsigned nfft, float* dest, const float* src);
    static int bcr1d (unsigned nfft, float* dest, const float* src);

    //! Agent class
    class Agent : public PlanAgent<FFTW_Plan> {
    public:
      Agent () : PlanAgent<FFTW_Plan> ("FFTW", nfft) { }
      FFTW_Plan* new_plan (unsigned nfft, const std::string& call);
    };

  protected:

    void* plan;
    float* tmp;

    static Agent my_agent;
  };

  class FFTW_Plan2 : public Plan2 {

  public:
    FFTW_Plan2 ();
    FFTW_Plan2 (unsigned nx, unsigned ny, const std::string& call);
    ~FFTW_Plan2 ();
    void init (unsigned nx, unsigned ny, const std::string& call);

    static void
    fcc2d (unsigned nx, unsigned ny, float* dest, const float* src);
    static void
    bcc2d (unsigned nx, unsigned ny, float* dest, const float* src);

  protected:
    void* plan;

    class Agent : public PlanAgent2<FFTW_Plan2> {
    public:
      Agent () : PlanAgent2<FFTW_Plan2> ("FFTW", nfft) { }
      FFTW_Plan2* new_plan (unsigned nx, unsigned ny, const std::string& call);
    };

    static Agent my_agent;
  };

}

#endif
