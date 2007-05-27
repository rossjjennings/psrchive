/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MKL_DFTI_Transform.h"
#include "Error.h"

using namespace std;

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::MKL_DFTI::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan nfft=" << n
       << " call='" << fft_call << "'" << endl;
#endif

  int status;

  long n = n_fft;

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

  this->nfft = n_fft;
  this->call = t;
  this->optimized = optimize;
}

FTransform::MKL_DFTI::Plan::~Plan()
{
  if (plan)
    DftiFreeDescriptor(&plan);
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

/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */


FTransform::MKL_DFTI::Plan2::Plan2 (size_t n_x, size_t n_y, type t)
{
  long n[2];
  n[0] = n_x;
  n[1] = n_y;

  int status;

  if (t & real)
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_REAL, 1, n );
  else
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_COMPLEX, 1, n );

  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		 "fail DftiCreateDescriptor");

#if 0

  // this code is in the double precision version in
  // interfaces/fftw3xc/wrappers
  if (t & real) {
    status = DftiSetValue
      ( plan, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX );

    if (! DftiErrorClass(status, DFTI_NO_ERROR))
      throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		   "fail DftiSetValue DFTI_CONJUGATE_EVEN_STORAGE");
  }

#endif

  status = DftiSetValue( plan, DFTI_PLACEMENT, DFTI_NOT_INPLACE );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		 "fail DftiSetValue DFTI_PLACEMENT");

  status = DftiCommitDescriptor( plan );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		 "fail DftiCommitDescriptor");

  this->nx = n_x;
  this->ny = n_y;
  this->call = t;
}

FTransform::MKL_DFTI::Plan2::~Plan2 ()
{
  if (plan)
    DftiFreeDescriptor(&plan);
}

void FTransform::MKL_DFTI::Plan2::fcc2d (size_t nx, size_t ny,
					 float* dest, const float* src)
{
  DftiComputeForward( plan, (void*) src, dest );
}


void FTransform::MKL_DFTI::Plan2::bcc2d (size_t nx, size_t ny,
					 float* dest, const float* src)
{
  DftiComputeBackward( plan, (void*) src, dest );
}

