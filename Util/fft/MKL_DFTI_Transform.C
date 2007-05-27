/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_MKL_DFTI

#include "MKL_DFTI_Transform.h"
#include "Error.h"

using namespace std;

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::MKL_DFTI::Plan::~Plan()
{
  if (plan)
    DftiFreeDescriptor(&plan);
}

FTransform::MKL_DFTI::Plan::Plan (size_t n, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan nfft=" << n
       << " call='" << fft_call << "'" << endl;
#endif

  int status;

  if (t & real)
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_REAL, 1, n );
  else
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_COMPLEX, 1, n );

  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		 "fail DftiCreateDescriptor");

#if 0

  // this code is in the double precision version in
  // interfaces/fftw3xc/wrappers
  if (t & real) {
    status = DftiSetValue
      ( plan, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX );

    if (! DftiErrorClass(status, DFTI_NO_ERROR))
      throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		   "fail DftiSetValue DFTI_CONJUGATE_EVEN_STORAGE");
  }

#endif

  status = DftiSetValue( plan, DFTI_PLACEMENT, DFTI_NOT_INPLACE );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		 "fail DftiSetValue DFTI_PLACEMENT");

  status = DftiCommitDescriptor( plan );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		 "fail DftiCommitDescriptor");

  this->nfft = n;
  this->call = t;
  this->optimized = optimize;
}

void FTransform::MKL_DFTI::Plan::frc1d (size_t nfft, float* dest, 
					const float* src)
{
  DftiComputeForward( plan, (void*) src, dest );
}

void FTransform::MKL_DFTI::Plan::fcc1d (size_t nfft, float* dest,
					const float* src)
{
  DftiComputeForward( plan, (void*) src, dest );
}

void FTransform::MKL_DFTI::Plan::bcc1d (size_t nfft, float* dest,
					const float* src)
{
  DftiComputeBackward( plan, (void*) src, dest );
}

void FTransform::MKL_DFTI::Plan::bcr1d (size_t nfft, float* dest,
					const float* src)
{
  DftiComputeBackward( plan, (void*) src, dest );
}

#if 0

/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */


FTransform::MKL_DFTI::Plan2::Plan2 (size_t n_x, size_t n_y, type t)
{
  fftw_direction direction_flags;
  if( t & forward )
    direction_flags = MKL_DFTI_FORWARD;
  else
    direction_flags = MKL_DFTI_BACKWARD;

  int flags = 0;
  if (optimized)
    flags |= MKL_DFTI_MEASURE;
  else
    flags |= MKL_DFTI_ESTIMATE;

  plan = fftw2d_create_plan (n_x, n_y, direction_flags, flags);
  nx = n_x;
  ny = n_y;
  call = t;

}

FTransform::MKL_DFTI::Plan2::~Plan2 ()
{
  fftwnd_destroy_plan ((fftwnd_plan)plan);
}

void FTransform::MKL_DFTI::Plan2::fcc2d (size_t nx, size_t ny,
					 float* dest, const float* src)
{
  fftwnd_one ((fftwnd_plan)plan, (fftw_complex*)src, (fftw_complex*)dest);
}


void FTransform::MKL_DFTI::Plan2::bcc2d (size_t nx, size_t ny,
					 float* dest, const float* src)
{
  fftwnd_one ((fftwnd_plan)plan, (fftw_complex*)src, (fftw_complex*)dest);
}

#endif

#endif
