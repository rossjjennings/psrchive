#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW3

#include "Error.h"

#include "FFTW3_Transform.h"

int FTransform::fftw3_initialise(){
  frc1d_calls.push_back( &fftw3_frc1d );
  fcc1d_calls.push_back( &fftw3_fcc1d );
  bcc1d_calls.push_back( &fftw3_bcc1d );
  bcr1d_calls.push_back( &fftw3_bcr1d );

  norms.push_back( nfft );
  valid_libraries.push_back( "FFTW3" );

  if( library==string() ){
    library = "FFTW3";
    frc1d = &fftw3_frc1d;
    fcc1d = &fftw3_fcc1d;
    bcc1d = &fftw3_bcc1d;
    bcr1d = &fftw3_bcr1d;
    norm = norms.back();
  }

  return 0;
}

FTransform::FFTW3_Plan::~FFTW3_Plan(){
  if( plan ){
    fftwf_destroy_plan(*plan);
    delete plan;
  }
}

FTransform::FFTW3_Plan::FFTW3_Plan() : Plan() {
  plan = 0;
}

FTransform::FFTW3_Plan::FFTW3_Plan(unsigned _ndat, unsigned _ilib, string _fft_call)
  : Plan(_ndat,_ilib,_fft_call)
{
  init(ndat,ilib,fft_call);
}

void FTransform::FFTW3_Plan::init(unsigned _ndat, unsigned _ilib, string _fft_call)
{
  fprintf(stderr,"In FTransform::FFTW3_Plan::init() _ndat=%d _ilib=%d _fft_call='%s'\n",
	  _ndat,_ilib,_fft_call.c_str());

  initialise(_ndat,_ilib,_fft_call);

  int direction_flags = 0;
  if( fft_call == "frc1d" || fft_call == "fcc1d" )
    direction_flags |= FFTW_FORWARD;
  else
    direction_flags |= FFTW_BACKWARD;

  int flags = FFTW_UNALIGNED;
  if (optimized)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  plan = new fftwf_plan;

  float* in = new float[ndat+2];
  float* out = new float[ndat+2];

  if( fft_call == "frc1d" )
    *plan = fftwf_plan_dft_r2c_1d(ndat, in, (fftwf_complex*)out, flags);
  else if( fft_call == "bcr1d" )
    *plan = fftwf_plan_dft_c2r_1d(ndat, (fftwf_complex*)in, out, flags);
  else
    *plan = fftwf_plan_dft_1d(ndat,(fftwf_complex*)in,(fftwf_complex*)out,
			      direction_flags, flags);

  delete [] in;
  delete [] out;
}

int FTransform::fftw3_frc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW3");
  FFTW3_Plan* plan = (FFTW3_Plan*)last_frc1d_plan;

  if( !last_frc1d_plan || 
      last_frc1d_plan->ilib != ilib || 
      last_frc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "frc1d"){
	plan = (FFTW3_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW3_Plan(ndat,ilib,"frc1d");

  last_frc1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  fftwf_execute_dft_r2c( *plan->plan, src, (fftwf_complex*)dest);

  return 0;
}

int FTransform::fftw3_fcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW3");
  FFTW3_Plan* plan = (FFTW3_Plan*)last_fcc1d_plan;

  if( !last_fcc1d_plan || 
      last_fcc1d_plan->ilib != ilib || 
      last_fcc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "fcc1d"){
	plan = (FFTW3_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW3_Plan(ndat,ilib,"fcc1d");

  last_fcc1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  fftwf_execute_dft( *plan->plan, (fftwf_complex*) src, (fftwf_complex*) dest);

  return 0;
}

int FTransform::fftw3_bcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW3");
  FFTW3_Plan* plan = (FFTW3_Plan*)last_bcc1d_plan;

  if( !last_bcc1d_plan || 
      last_bcc1d_plan->ilib != ilib || 
      last_bcc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "bcc1d"){
	plan = (FFTW3_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW3_Plan(ndat,ilib,"bcc1d");

  last_bcc1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  fftwf_execute_dft( *plan->plan, (fftwf_complex*) src, (fftwf_complex*) dest);

  return 0;
}

int FTransform::fftw3_bcr1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW3");
  FFTW3_Plan* plan = (FFTW3_Plan*)last_bcr1d_plan;

  if( !last_bcr1d_plan || 
      last_bcr1d_plan->ilib != ilib || 
      last_bcr1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "bcr1d"){
	plan = (FFTW3_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW3_Plan(ndat,ilib,"bcr1d");

  last_bcr1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  fftwf_execute_dft_c2r( *plan->plan, (fftwf_complex*)src, dest);

  return 0;
}


#endif
