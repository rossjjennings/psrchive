//-*-C++-*-

#ifndef _utils_psrfft_FFTW3_Transform_h_
#define _utils_psrfft_FFTW3_Transform_h_

#include "FTransform.h"

#include <fftw3.h>

namespace FTransform {

  int fftw3_initialise();

  int fftw3_frc1d(unsigned ndat, float* dest, const float* src);
  int fftw3_fcc1d(unsigned ndat, float* dest, const float* src);
  int fftw3_bcc1d(unsigned ndat, float* dest, const float* src);
  int fftw3_bcr1d(unsigned ndat, float* dest, const float* src);

  class FFTW3_Plan : public Plan {
  public:
    FFTW3_Plan();
    FFTW3_Plan(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);
    ~FFTW3_Plan();
    void init(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);

    fftwf_plan* plan;
  };

  class FFTW3_Plan2 : public Plan2 {

  public:
    FFTW3_Plan2 ();
    FFTW3_Plan2 (unsigned nx, unsigned ny, const std::string& _fft_call);
    ~FFTW3_Plan2 ();
    void init (unsigned nx, unsigned ny, const std::string& call);

    static void fcc2d (unsigned nx, unsigned ny, float* dest, const float* src);
    static void bcc2d (unsigned nx, unsigned ny, float* dest, const float* src);

  protected:
    void* plan;

    class Agent : public PlanAgent2<FFTW3_Plan2> {
    public:
      Agent () : PlanAgent2<FFTW3_Plan2> ("FFTW3", nfft) { }
      FFTW3_Plan2* new_plan (unsigned nx, unsigned ny, const std::string& cal);
    };

    static Agent my_agent;
  };

}

#endif
