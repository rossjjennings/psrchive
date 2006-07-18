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
#include "genutil.h"

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

FTransform::MKL_Plan::MKL_Plan (unsigned nfft, const string& fft_call)
  : Plan()
{
#ifdef _DEBUG
  cerr << "FTransform::MKL_Plan nfft=" << nfft
       << " call='" << fft_call << "'" << endl;
#endif

  if( fft_call == "frc1d" )
    mkl_plan = new float[2*nfft+10];
  else
    mkl_plan = new float[nfft*4];
  assert( mkl_plan != 0 );

  int signed_ndat = nfft;

  int isign = 0;
  if( fft_call == "frc1d" )
    scfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else if( fft_call == "bcr1d" )
    csfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else
    cfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);

  ndat = nfft;
  call = fft_call;
  optimized = false;

}

int FTransform::MKL_Plan::frc1d (unsigned nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, frc1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_nfft = nfft;

  if( dest != src )
    memcpy (dest, src, nfft*sizeof(float));
  scfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::MKL_Plan::fcc1d (unsigned nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, fcc1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_nfft = nfft;

  memcpy (dest, src, nfft*2*sizeof(float));
  cfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::MKL_Plan::bcc1d (unsigned nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, bcc1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = 1;
  int signed_nfft = nfft;

  memcpy (dest, src, nfft*2*sizeof(float));
  cfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::MKL_Plan::bcr1d (unsigned nfft, float* dest, const float* src)
{
  FT_SETUP (MKL_Plan, bcr1d);

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_nfft = nfft;

  memcpy (dest, src, nfft*sizeof(float));
  csfft1d_(dest, &signed_nfft, &isign, plan->mkl_plan);

  return 0;
}

FTransform::MKL_Plan::Agent::Agent () : 
  PlanAgent<MKL_Plan> ("MKL", nfft) 
{ 
}

FTransform::MKL_Plan::Agent::~Agent (){}

#endif
