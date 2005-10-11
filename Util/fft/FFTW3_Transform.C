#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW3

#include "FFTW3_Transform.h"
#include "Error.h"

using namespace std;

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::FFTW3_Plan::FFTW3_Plan (unsigned nfft, const string& fft_call)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW3_Plan nfft=" << nfft
       << " call='" << fft_call << "'" << endl;
#endif

  int direction_flags = 0;
  if( fft_call == "frc1d" || fft_call == "fcc1d" )
    direction_flags |= FFTW_FORWARD;
  else
    direction_flags |= FFTW_BACKWARD;

  int flags = FFTW_UNALIGNED;
  if (optimize)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  fftwf_complex* in = new fftwf_complex[nfft];
  fftwf_complex* out = new fftwf_complex[nfft];

  if( fft_call == "frc1d" )
    plan = fftwf_plan_dft_r2c_1d (nfft, (float*)in, out, flags);
  else if( fft_call == "bcr1d" )
    plan = fftwf_plan_dft_c2r_1d (nfft, in, (float*)out, flags);
  else
    plan = fftwf_plan_dft_1d (nfft, in, out, direction_flags, flags);

  ndat = nfft;
  call = fft_call;
  optimized = optimize;

  delete [] in;
  delete [] out;
}

FTransform::FFTW3_Plan::~FFTW3_Plan()
{
  if (plan)
    fftwf_destroy_plan ((fftwf_plan)plan);
}

int FTransform::FFTW3_Plan::frc1d (unsigned nfft,
				   float* dest, const float* src)
{
  FT_SETUP (FFTW3_Plan, frc1d);

  fftwf_execute_dft_r2c ((fftwf_plan)(plan->plan),
			 (float*)src, (fftwf_complex*)dest);

  return 0;
}

int FTransform::FFTW3_Plan::fcc1d (unsigned nfft,
				   float* dest, const float* src)
{
  FT_SETUP (FFTW3_Plan, fcc1d);

  fftwf_execute_dft ((fftwf_plan)(plan->plan),
		     (fftwf_complex*) src, (fftwf_complex*) dest);

  return 0;
}

int FTransform::FFTW3_Plan::bcc1d (unsigned nfft,
				   float* dest, const float* src)
{
  FT_SETUP (FFTW3_Plan, bcc1d);

  fftwf_execute_dft ((fftwf_plan)(plan->plan), 
		     (fftwf_complex*) src, (fftwf_complex*) dest);

  return 0;
}

int FTransform::FFTW3_Plan::bcr1d (unsigned nfft,
				   float* dest, const float* src)
{
  FT_SETUP (FFTW3_Plan, bcr1d);

  fftwf_execute_dft_c2r ((fftwf_plan)(plan->plan), (fftwf_complex*)src, dest);

  return 0;
}



/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */

FTransform::FFTW3_Plan2::Agent FTransform::FFTW3_Plan2::my_agent;

FTransform::FFTW3_Plan2::FFTW3_Plan2 (unsigned n_x, unsigned n_y,
				      const std::string& fft_call)
{
  int direction_flags = 0;
  if( fft_call == "fcc2d" )
    direction_flags |= FFTW_FORWARD;
  else
    direction_flags |= FFTW_BACKWARD;

  int flags = FFTW_UNALIGNED;
  if (optimized)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  fftwf_complex* in = new fftwf_complex[n_x*n_y];
  fftwf_complex* out = new fftwf_complex[n_x*n_y];

  plan = fftwf_plan_dft_2d (n_x, n_y, in, out, direction_flags, flags);
  nx = n_x;
  ny = n_y;
  call = fft_call;
  optimized = optimize;

  delete [] in;
  delete [] out;
}

FTransform::FFTW3_Plan2::~FFTW3_Plan2 ()
{
  fftwf_destroy_plan ((fftwf_plan)plan);
}

void FTransform::FFTW3_Plan2::fcc2d (unsigned nx, unsigned ny,
				     float* dest, const float* src)
{
  FT_SETUP2 (FFTW3_Plan2, fcc2d);

  fftwf_execute_dft ((fftwf_plan)(plan->plan),
		     (fftwf_complex*)src, (fftwf_complex*)dest);
}


void FTransform::FFTW3_Plan2::bcc2d (unsigned nx, unsigned ny,
				     float* dest, const float* src)
{
  FT_SETUP2 (FFTW3_Plan2, bcc2d);

  fftwf_execute_dft ((fftwf_plan)(plan->plan),
		     (fftwf_complex*)src, (fftwf_complex*)dest);
}


#endif
