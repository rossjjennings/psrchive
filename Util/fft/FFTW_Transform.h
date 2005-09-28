//-*-C++-*-

#ifndef _utils_psrfft_FFTW_Transform_h_
#define _utils_psrfft_FFTW_Transform_h_

#include "FTransform.h"

namespace FTransform {

  int fftw_initialise();

  int fftw_frc1d(unsigned ndat, float* dest, const float* src);
  int fftw_fcc1d(unsigned ndat, float* dest, const float* src);
  int fftw_bcc1d(unsigned ndat, float* dest, const float* src);
  int fftw_bcr1d(unsigned ndat, float* dest, const float* src);

  class FFTW_Plan : public Plan {
  public:
    FFTW_Plan();
    FFTW_Plan(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);
    ~FFTW_Plan();
    void init(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);
    
    void* plan;
    float* tmp;
  };


  class FFTW_Plan2 : public Plan2 {

  public:
    FFTW_Plan2 ();
    FFTW_Plan2 (unsigned nx, unsigned ny, const std::string& _fft_call);
    ~FFTW_Plan2 ();
    void init (unsigned nx, unsigned ny, const std::string& call);

    static void fcc2d (unsigned nx, unsigned ny, float* dest, const float* src);
    static void bcc2d (unsigned nx, unsigned ny, float* dest, const float* src);

  protected:
    void* plan;

    class Agent : public PlanAgent2<FFTW_Plan2> {
    public:
      Agent () : PlanAgent2<FFTW_Plan2> ("FFTW", nfft) { }
      FFTW_Plan2* new_plan (unsigned nx, unsigned ny, const std::string& cal);
    };

    static Agent my_agent;
  };

}

#endif
