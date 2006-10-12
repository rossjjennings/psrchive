/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW

#include "FFTW_Transform.h"
#include "Error.h"

#include <rfftw.h>
#include <assert.h>

using namespace std;

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::FFTW::Plan::~Plan()
{
  if (plan) {

    if ( call & real )
      rfftwnd_destroy_plan ((rfftwnd_plan) plan);
    else
      fftw_destroy_plan ((fftw_plan) plan);

  }

  if (tmp) delete [] tmp;

  // cerr << "FFTW::Plan destroyed" << endl;
}

FTransform::FFTW::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW::Plan nfft=" << n_fft
       << " call='" << fft_call << "'" << endl;
#endif

  tmp = 0;

  fftw_direction wdir;

  if( t & forward )
    wdir = FFTW_FORWARD;
  else
    wdir = FFTW_BACKWARD;

  int flags = FFTW_ESTIMATE;
  if( optimize )
    flags = FFTW_MEASURE;

  if (t & real) {
    int int_nfft = n_fft;
    plan = rfftwnd_create_plan (1, &int_nfft, wdir, flags);
    tmp = new float[n_fft+2];
    assert( tmp != 0 );
  }
  else
    plan = fftw_create_plan (n_fft, wdir, flags);

  this->nfft = n_fft;
  this->call = t;
  this->optimized = optimize;
}

void FTransform::FFTW::Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  rfftwnd_one_real_to_complex ((rfftwnd_plan)plan,
			       (fftw_real*)src, (fftw_complex*)dest);
}

void FTransform::FFTW::Plan::fcc1d (size_t nfft, float* dest, const float* src)
{
  fftw_one ((fftw_plan)plan, (fftw_complex*)src, (fftw_complex*)dest);
}

void FTransform::FFTW::Plan::bcc1d (size_t nfft, float* dest, const float* src)
{
  fftw_one ((fftw_plan)plan, (fftw_complex*)src, (fftw_complex*)dest);
}

void FTransform::FFTW::Plan::bcr1d (size_t nfft, float* dest, const float* src)
{
  rfftwnd_one_complex_to_real ((rfftwnd_plan)plan, 
			       (fftw_complex*)src, (fftw_real*)dest);
}


/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */


FTransform::FFTW::Plan2::Plan2 (size_t n_x, size_t n_y, type t)
{
  fftw_direction direction_flags;
  if( t & forward )
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
  call = t;

}

FTransform::FFTW::Plan2::~Plan2 ()
{
  fftwnd_destroy_plan ((fftwnd_plan)plan);
}

void FTransform::FFTW::Plan2::fcc2d (size_t nx, size_t ny,
				     float* dest, const float* src)
{
  fftwnd_one ((fftwnd_plan)plan, (fftw_complex*)src, (fftw_complex*)dest);
}


void FTransform::FFTW::Plan2::bcc2d (size_t nx, size_t ny,
				     float* dest, const float* src)
{
  fftwnd_one ((fftwnd_plan)plan, (fftw_complex*)src, (fftw_complex*)dest);
}


#endif
