#if HAVE_FFTW

#include "Error.h"

#include "FFTW_Transform.h"

int Transform::fftw_initialise(){
  frc1d_calls.push_back( &fftw_frc1d );
  fcc1d_calls.push_back( &fftw_fcc1d );
  bcc1d_calls.push_back( &fftw_bcc1d );

  norms.push_back( nfft );
  valid_libraries.push_back( "FFTW" );

  if( library==string() ){
    library = "FFTW";
    frc1d = &fftw_frc1d;
    fcc1d = &fftw_fcc1d;
    bcc1d = &fftw_bcc1d;
    norm = norms.back();
  }

  return 0;
}

Transform::FFTW_Plan::~FFTW_Plan(){
  if( plan ){
    if ( fft_call == "frc1d" ){
      rfftw_destroy_plan (*(rfftw_plan*) plan);
      delete (fftw_plan*)plan;
    }
    else{
      fftw_destroy_plan (*(fftw_plan*) plan);
      delete (rfftw_plan*)plan;
    }
  }
  if( tmp )
    delete [] tmp;

}

Transform::FFTW_Plan::FFTW_Plan() : Plan() {
  plan = 0;
  tmp = 0;
}

Transform::FFTW_Plan::FFTW_Plan(unsigned _ndat, unsigned _ilib, string _fft_call)
  : Plan(_ndat,_ilib,_fft_call)
{
  init(ndat,ilib,fft_call);
}

void Transform::FFTW_Plan::init(unsigned _ndat, unsigned _ilib, string _fft_call){
  initialise(_ndat,_ilib,_fft_call);
  tmp = 0;

  fftw_direction wdir;

  if( fft_call == "frc1d" || fft_call == "fcc1d" )
    wdir = FFTW_FORWARD;
  else
    wdir = FFTW_BACKWARD;

  int flags = FFTW_ESTIMATE;
  if( optimized )
    flags = FFTW_MEASURE;

  if( fft_call == "frc1d" ){
    plan = (void*) new rfftw_plan;
    *(rfftw_plan*)plan = rfftw_create_plan (ndat, wdir, flags);
  }
  else {
    plan = (void*) new fftw_plan;
    *(fftw_plan*)plan = fftw_create_plan (ndat, wdir, flags);
  }
}

int Transform::fftw_frc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = 0;

  if( last_frc1d_plan && last_frc1d_plan->ilib==ilib &&
      last_frc1d_plan->ndat == ndat )
    plan = (FFTW_Plan*)last_frc1d_plan;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "frc1d"){
	plan = (FFTW_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW_Plan(ndat,ilib,"frc1d");

  ///////////////////////////////////////
  // Do the transform
  if( !plan->tmp ){
    plan->tmp = new float[ndat+2];
    assert( plan->tmp != 0 );
  }

  rfftw_one (*(rfftw_plan*)plan->plan, (fftw_real*)src,
	     (fftw_real*)plan->tmp);
  rfftw_resort(ndat, plan->tmp, dest);

  return 0;
}

int Transform::fftw_fcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = 0;

  if( last_fcc1d_plan && last_fcc1d_plan->ilib==ilib &&
      last_fcc1d_plan->ndat == ndat )
    plan = (FFTW_Plan*)last_fcc1d_plan;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "fcc1d"){
	plan = (FFTW_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan ){
    plan = new FFTW_Plan(ndat,ilib,"fcc1d");
    plans[ilib].push_back( plan );
  }

  ///////////////////////////////////////
  // Do the transform
  fftw_one(*(fftw_plan*)plan->plan,(fftw_complex*)src,(fftw_complex*)dest);

  return 0;
}

int Transform::fftw_bcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = 0;

  if( last_bcc1d_plan && last_bcc1d_plan->ilib==ilib &&
      last_bcc1d_plan->ndat == ndat )
    plan = (FFTW_Plan*)last_bcc1d_plan;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "bcc1d"){
	plan = (FFTW_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan ){
    plan = new FFTW_Plan(ndat,ilib,"bcc1d");
    plans[ilib].push_back( plan );
  }

  ///////////////////////////////////////
  // Do the transform
  fftw_one(*(fftw_plan*)plan->plan,(fftw_complex*)src,(fftw_complex*)dest);

  return 0;
}

#endif
