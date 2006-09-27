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

FTransform::MKL_Plan::MKL_Plan (size_t nfft, const string& fft_call)
  : Plan()
{
  fprintf(stderr,"In MKL_Plan constructor(%d,%s)\n",
	  nfft, fft_call.c_str());

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

  if( int64(uint64(nfft)) != signed_ndat )
    throw Error(InvalidState,"FTransform::MKL_Plan::MKL_Plan()",
		"Could not convert nfft="UI64" to an integer",
		uint64(nfft));

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

int FTransform::MKL_Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  //  fprintf(stderr,"In FTransform::MKL_Plan::frc1d(%d)\n",nfft);
  //  fprintf(stderr,"yo1.0 last_frc1d=%p\n",last_frc1d);
  //FT_SETUP (MKL_Plan, frc1d);
  MKL_Plan* plan = dynamic_cast<MKL_Plan*>( last_frc1d );
  if (!plan || plan->ndat != nfft || plan->call != "frc1d")
    last_frc1d = plan = Agent::my_agent.get_plan (nfft, "frc1d");

  //  fprintf(stderr,"In FTransform::MKL_Plan::frc1d(%d) with plan=%p\n",nfft,plan);

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
  PlanAgent<MKL_Plan> ("MKL", nfft) 
{ 
}

FTransform::MKL_Plan::Agent::~Agent (){}

#endif
