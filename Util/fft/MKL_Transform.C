/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_MKL

#include "MKL_Transform.h"
#include "environ.h"

#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;

extern "C" { 
  // complex to complex
  void cfft1d_(float * data, int * ndat, int * isign, float * wsave);
  // real to complex
  void scfft1d_(float * data, int * ndat, int * isign, float * wsave);
  // complex to real
  void csfft1d_(float * data, int * ndat, int * isign, float * wsave);
}


FTransform::MKL_Plan::~MKL_Plan()
{
  if (mkl_plan)
    delete [] mkl_plan;
}

FTransform::MKL_Plan::MKL_Plan (size_t n_fft, const string& fft_call)
  : Plan()
{
  fprintf(stderr,"In MKL_Plan constructor(%d,%s)\n",
	  n_fft, fft_call.c_str());

#ifdef _DEBUG
  cerr << "FTransform::MKL_Plan nfft=" << n_fft
       << " call='" << fft_call << "'" << endl;
#endif

  if( fft_call == "frc1d" )
    mkl_plan = new float[2*n_fft+10];
  else
    mkl_plan = new float[n_fft*4];
  assert( mkl_plan != 0 );

  int signed_ndat = n_fft;

  if( int64(uint64(n_fft)) != signed_ndat )
    throw Error(InvalidState,"FTransform::MKL_Plan::MKL_Plan()",
		"Could not convert nfft="UI64" to an integer",
		uint64(n_fft));

  int isign = 0;
  if( fft_call == "frc1d" )
    scfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else if( fft_call == "bcr1d" )
    csfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else
    cfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);

  nfft = n_fft;
  call = fft_call;
  optimized = false;

}

int FTransform::MKL_Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, frc1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_nfft = nfft;

  if( int64(uint64(nfft)) != signed_nfft )
    throw Error(InvalidState,"FTransform::MKL_Plan::frc1d()",
		"Could not convert nfft="UI64" to an integer",
		uint64(nfft));

  if( dest != src )
    memcpy (dest, src, nfft*sizeof(float));

  scfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::MKL_Plan::fcc1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, fcc1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_nfft = nfft;

  if( int64(uint64(nfft)) != signed_nfft )
    throw Error(InvalidState,"FTransform::MKL_Plan::fcc1d()",
		"Could not convert nfft="UI64" to an integer",
		uint64(nfft));

  memcpy (dest, src, nfft*2*sizeof(float));
  cfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::MKL_Plan::bcc1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, bcc1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = 1;
  int signed_nfft = nfft;

  if( int64(uint64(nfft)) != signed_nfft )
    throw Error(InvalidState,"FTransform::MKL_Plan::bcc1d()",
		"Could not convert nfft="UI64" to an integer",
		uint64(nfft));

  memcpy (dest, src, nfft*2*sizeof(float));
  cfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::MKL_Plan::bcr1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, bcr1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_nfft = nfft;

  if( int64(uint64(nfft)) != signed_nfft )
    throw Error(InvalidState,"FTransform::MKL_Plan::bcr1d()",
		"Could not convert nfft="UI64" to an integer",
		uint64(nfft));

  memcpy (dest, src, nfft*sizeof(float));
  csfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

FTransform::MKL_Plan::Agent::Agent () : 
  PlanAgent<MKL_Plan> ("MKL", normalized) 
{ 
}

FTransform::MKL_Plan::Agent::~Agent (){}

#endif
