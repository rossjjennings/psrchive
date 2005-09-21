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

int FTransform::fftw_initialise(){
  frc1d_calls.push_back( &fftw_frc1d );
  fcc1d_calls.push_back( &fftw_fcc1d );
  bcc1d_calls.push_back( &fftw_bcc1d );
  bcr1d_calls.push_back( &fftw_bcr1d );

  norms.push_back( nfft );
  valid_libraries.push_back( "FFTW" );

  if( library==string() ){
    library = "FFTW";
    frc1d = &fftw_frc1d;
    fcc1d = &fftw_fcc1d;
    bcc1d = &fftw_bcc1d;
    bcr1d = &fftw_bcr1d;
    norm = norms.back();
  }

  return 0;
}

FTransform::FFTW_Plan::~FFTW_Plan(){
  if( plan ){
    if ( fft_call == "frc1d" || fft_call == "brc1d" ){
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

FTransform::FFTW_Plan::FFTW_Plan() : Plan() {
  plan = 0;
  tmp = 0;
}

FTransform::FFTW_Plan::FFTW_Plan(unsigned _ndat, unsigned _ilib, const string& _fft_call)
  : Plan(_ndat,_ilib,_fft_call)
{
  init(ndat,ilib,fft_call);
}

void FTransform::FFTW_Plan::init(unsigned _ndat, unsigned _ilib, const string& _fft_call)
{
  fprintf(stderr,"In FTransform::FFTW_Plan::init() _ndat=%d _ilib=%d _fft_call='%s'\n",
	  _ndat,_ilib,_fft_call.c_str());

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

  if( fft_call == "frc1d" || fft_call == "bcr1d" ){
    plan = (void*) new rfftw_plan;
    *(rfftw_plan*)plan = rfftw_create_plan (ndat, wdir, flags);
  }
  else {
    plan = (void*) new fftw_plan;
    *(fftw_plan*)plan = fftw_create_plan (ndat, wdir, flags);
  }
}

int FTransform::fftw_frc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = (FFTW_Plan*)last_frc1d_plan;

  if( !last_frc1d_plan || 
      last_frc1d_plan->ilib != ilib || 
      last_frc1d_plan->ndat != ndat )
    plan = 0;

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

  last_frc1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  if( !plan->tmp ){
    plan->tmp = new float[ndat+2];
    assert( plan->tmp != 0 );
  }

  rfftw_one (*(rfftw_plan*)plan->plan, (fftw_real*)src,
	     (fftw_real*)plan->tmp);
  rfftw_sort(ndat, plan->tmp, dest);

  return 0;
}

int FTransform::fftw_fcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = (FFTW_Plan*)last_fcc1d_plan;

  if( !last_fcc1d_plan || 
      last_fcc1d_plan->ilib != ilib || 
      last_fcc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "fcc1d"){
	plan = (FFTW_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW_Plan(ndat,ilib,"fcc1d");

  last_fcc1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  fftw_one(*(fftw_plan*)plan->plan,(fftw_complex*)src,(fftw_complex*)dest);

  return 0;
}

int FTransform::fftw_bcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = (FFTW_Plan*)last_bcc1d_plan;

  if( !last_bcc1d_plan || 
      last_bcc1d_plan->ilib != ilib || 
      last_bcc1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "bcc1d"){
	plan = (FFTW_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW_Plan(ndat,ilib,"bcc1d");

  last_bcc1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  fftw_one(*(fftw_plan*)plan->plan,(fftw_complex*)src,(fftw_complex*)dest);

  return 0;
}

int FTransform::fftw_bcr1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = get_ilib("FFTW");
  FFTW_Plan* plan = (FFTW_Plan*)last_bcr1d_plan;

  if( !last_bcr1d_plan || 
      last_bcr1d_plan->ilib != ilib || 
      last_bcr1d_plan->ndat != ndat )
    plan = 0;

  if( !plan ){
    for( unsigned iplan=0; iplan<plans[ilib].size(); iplan++){
      if( plans[ilib][iplan]->ndat == ndat && 
	  plans[ilib][iplan]->fft_call == "bcr1d"){
	plan = (FFTW_Plan*)plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new FFTW_Plan(ndat,ilib,"bcr1d");

  last_bcr1d_plan = plan;

  ///////////////////////////////////////
  // Do the transform
  if( !plan->tmp ){
    plan->tmp = new float[ndat+2];
    assert( plan->tmp != 0 );
  }

  throw Error(InvalidState,"FTransform::fftw_bcr1d()",
	      "rfftw_antisort was never written!  What are you using FFTW 2 for anyway?  FFTW3 is out and it's free so use that!");
  //rfftw_antisort(ndat, plan->tmp, dest);
  rfftw_one (*(rfftw_plan*)plan->plan, (fftw_real*)src,
	     (fftw_real*)plan->tmp);

  return 0;
}






















FTransform::FFTW_Plan2::Agent FTransform::FFTW_Plan2::my_agent;

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
				     float* dest, float* src)
{
  FFTW_Plan2* plan = dynamic_cast<FFTW_Plan2*>(last_fcc2d_plan);

  if (!plan || plan->nx != nx || plan->ny != ny || plan->call != "fcc2d")
    last_fcc2d_plan = plan = my_agent.get_plan (nx, ny, "fcc2d");

  fftwnd_one ((fftwnd_plan)(plan->plan),
	      (fftw_complex*)src, (fftw_complex*)dest);
}


void FTransform::FFTW_Plan2::bcc2d (unsigned nx, unsigned ny,
				     float* dest, float* src)
{
  FFTW_Plan2* plan = dynamic_cast<FFTW_Plan2*>(last_bcc2d_plan);

  if (!plan || plan->nx != nx || plan->ny != ny || plan->call != "bcc2d")
    last_bcc2d_plan = plan = my_agent.get_plan (nx, ny, "bcc2d");

  fftwnd_one ((fftwnd_plan)(plan->plan),
	      (fftw_complex*)src, (fftw_complex*)dest);
}


#endif
