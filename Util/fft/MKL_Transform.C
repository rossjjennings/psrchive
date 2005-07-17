#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_MKL

#include <string>
#include <stdio.h>
#include "psr_cpp.h"

#include "MKL_Transform.h"

extern "C" { 
  // complex to complex
  void cfft1d_(float * data, int * ndat, int * isign, float * wsave);
  // real to complex
  void scfft1d_(float * data, int * ndat, int * isign, float * wsave);
}

FTransform::MKL_Plan::MKL_Plan() : Plan(){ 
  mkl_plan = 0;
}

int FTransform::mkl_initialise(){
  fprintf(stderr,"In FTransform::mkl_initialise()\n");

  frc1d_calls.push_back( &mkl_frc1d );
  fcc1d_calls.push_back( &mkl_fcc1d );
  bcc1d_calls.push_back( &mkl_bcc1d );

  norms.push_back( normal );
  valid_libraries.push_back( "MKL" );

  if( library==string() ){
    library = "MKL";
    frc1d = &mkl_frc1d;
    fcc1d = &mkl_fcc1d;
    bcc1d = &mkl_bcc1d;
    norm = norms.back();
  }

  return 0;
}

FTransform::MKL_Plan::~MKL_Plan(){
  if( mkl_plan )
    delete [] mkl_plan;
}

FTransform::MKL_Plan::MKL_Plan(unsigned _ndat, unsigned _ilib, string _fft_call)
  : Plan(_ndat,_ilib,_fft_call)
{
  init(ndat,ilib,fft_call);
}

void FTransform::MKL_Plan::init(unsigned _ndat, unsigned _ilib, string _fft_call)
{
  fprintf(stderr,"In FTransform::MKL_Plan::init()\n");

  initialise(_ndat,_ilib,_fft_call);

  mkl_plan = new float[ndat*4];
  assert( mkl_plan != 0 );

  int signed_ndat = ndat;

  int isign = 0;
  if( fft_call == "frc1d" )
    scfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else
    cfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
}

int FTransform::mkl_frc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("MKL");
  MKL_Plan* plan = (MKL_Plan*)last_frc1d_plan;

  if( !last_frc1d_plan || 
      last_frc1d_plan->ilib != ilib || 
      last_frc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "frc1d"){
	plan = (MKL_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new MKL_Plan(ndat,ilib,"frc1d");

  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_ndat = ndat;

  memcpy (dest, src, ndat*sizeof(float));
  scfft1d_(dest, &signed_ndat, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::mkl_fcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("MKL");
  MKL_Plan* plan = (MKL_Plan*)last_fcc1d_plan;

  if( !last_fcc1d_plan || 
      last_fcc1d_plan->ilib != ilib || 
      last_fcc1d_plan->ndat != ndat )
    plan = 0;
  
  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "fcc1d"){
	plan = (MKL_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan ){
    plan = new MKL_Plan(ndat,ilib,"fcc1d");
    plans[ilib].push_back( plan );
  }


  ///////////////////////////////////////
  // Do the transform
  int isign = -1;
  int signed_ndat = ndat;

  memcpy (dest, src, ndat*2*sizeof(float));
  cfft1d_(dest, &signed_ndat, &isign, plan->mkl_plan);

  return 0;
}

int FTransform::mkl_bcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("MKL");
  MKL_Plan* plan = (MKL_Plan*)last_bcc1d_plan;

  if( !last_bcc1d_plan || 
      last_bcc1d_plan->ilib != ilib || 
      last_bcc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "bcc1d"){
	plan = (MKL_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan ){
    plan = new MKL_Plan(ndat,ilib,"bcc1d");
    plans[ilib].push_back( plan );
  }

  ///////////////////////////////////////
  // Do the transform
  int isign = 1;
  int signed_ndat = ndat;

  memcpy (dest, src, ndat*2*sizeof(float));
  cfft1d_(dest, &signed_ndat, &isign, plan->mkl_plan);

  return 0;
}

#endif
