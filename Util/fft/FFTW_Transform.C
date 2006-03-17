/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW

#include "FFTW_Transform.h"
#include "Error.h"
#include "psrfft.h"

#include <rfftw.h>
#include <assert.h>

using namespace std;

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::FFTW_Plan::~FFTW_Plan()
{
  if (plan) {

    if ( call == "frc1d" || call == "brc1d" )
      rfftw_destroy_plan ((rfftw_plan) plan);

    else
      fftw_destroy_plan ((fftw_plan) plan);

  }

  if (tmp) delete [] tmp;

}

FTransform::FFTW_Plan::FFTW_Plan (unsigned nfft, const string& fft_call)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW_Plan nfft=" << nfft
       << " call='" << fft_call << "'" << endl;
#endif

  tmp = 0;

  fftw_direction wdir;

  if( fft_call == "frc1d" || fft_call == "fcc1d" )
    wdir = FFTW_FORWARD;
  else
    wdir = FFTW_BACKWARD;

  int flags = FFTW_ESTIMATE;
  if( optimize )
    flags = FFTW_MEASURE;

  if (fft_call == "frc1d" || fft_call == "bcr1d") {
    plan = rfftw_create_plan (ndat, wdir, flags);
    tmp = new float[ndat+2];
    assert( tmp != 0 );
  }
  else
    plan = fftw_create_plan (ndat, wdir, flags);

  ndat = nfft;
  call = fft_call;
  optimized = optimize;
}

int FTransform::FFTW_Plan::frc1d (unsigned nfft,
				  float* dest, const float* src)
{
  FT_SETUP (FFTW_Plan, frc1d);

  rfftw_one ((rfftw_plan)(plan->plan), 
	     (fftw_real*)src, (fftw_real*)plan->tmp);
  rfftw_sort (nfft, plan->tmp, dest);

  return 0;
}

int FTransform::FFTW_Plan::fcc1d (unsigned nfft,
				  float* dest, const float* src)
{
  FT_SETUP (FFTW_Plan, fcc1d);

  fftw_one ((fftw_plan)(plan->plan),
	    (fftw_complex*)src, (fftw_complex*)dest);

  return 0;
}

int FTransform::FFTW_Plan::bcc1d(unsigned ndat, float* dest, const float* src)
{
  FT_SETUP (FFTW_Plan, bcc1d);

  fftw_one ((fftw_plan)(plan->plan),
	    (fftw_complex*)src, (fftw_complex*)dest);

  return 0;
}

int FTransform::FFTW_Plan::bcr1d (unsigned ndat, float* dest, const float* src)
{
  FT_SETUP (FFTW_Plan, bcr1d);

  throw Error (InvalidState, "FTransform::FFTW_Plan::fftw_bcr1d",
	       "not implemented");

  rfftw_one ((rfftw_plan)(plan->plan),
	     (fftw_real*)src, (fftw_real*)plan->tmp);

  return 0;
}


/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */


FTransform::FFTW_Plan2::FFTW_Plan2 (unsigned n_x, unsigned n_y,
				    const std::string& fft_call)
{
  fftw_direction direction_flags;
  if( fft_call == "fcc2d" )
    direction_flags = FFTW_FORWARD;
  else
    direction_flags = FFTW_BACKWARD;

  int flags = 0;
  if (optimized)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  plan = fftw2d_create_plan (n_x, n_y, direction_flags, flags);
  nx = n_x;
  ny = n_y;
  call = fft_call;

}

FTransform::FFTW_Plan2::~FFTW_Plan2 ()
{
  fftwnd_destroy_plan ((fftwnd_plan)plan);
}

void FTransform::FFTW_Plan2::fcc2d (unsigned nx, unsigned ny,
				    float* dest, const float* src)
{
  FT_SETUP2 (FFTW_Plan2, fcc2d);

  fftwnd_one ((fftwnd_plan)(plan->plan),
	      (fftw_complex*)src, (fftw_complex*)dest);
}


void FTransform::FFTW_Plan2::bcc2d (unsigned nx, unsigned ny,
				     float* dest, const float* src)
{
  FT_SETUP2 (FFTW_Plan2, bcc2d);

  fftwnd_one ((fftwnd_plan)(plan->plan),
	      (fftw_complex*)src, (fftw_complex*)dest);
}


#endif
