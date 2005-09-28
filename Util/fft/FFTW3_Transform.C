#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW3

#include "FFTW3_Transform.h"
#include "Error.h"

using namespace std;

int FTransform::fftw3_initialise()
{
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

FTransform::FFTW3_Plan::FFTW3_Plan(unsigned _ndat, unsigned _ilib, const string& _fft_call)
  : Plan(_ndat,_ilib,_fft_call)
{
  init(ndat,ilib,fft_call);
}

void FTransform::FFTW3_Plan::init(unsigned _ndat, unsigned _ilib, const string& _fft_call)
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

int FTransform::fftw3_frc1d(unsigned ndat, float* dest, const float* src){
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
  fftwf_execute_dft_r2c( *plan->plan, (float*)src, (fftwf_complex*)dest);

  return 0;
}

int FTransform::fftw3_fcc1d(unsigned ndat, float* dest, const float* src){
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

int FTransform::fftw3_bcc1d(unsigned ndat, float* dest, const float* src){
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

int FTransform::fftw3_bcr1d(unsigned ndat, float* dest, const float* src){
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




















FTransform::FFTW3_Plan2::Agent FTransform::FFTW3_Plan2::my_agent;

FTransform::FFTW3_Plan2::FFTW3_Plan2 (unsigned n_x, unsigned n_y,
				      const std::string& fft_call)
{
  int direction_flags = 0;
  if( fft_call == "fcc2d" )
    direction_flags |= FFTW_FORWARD;
  else
    direction_flags |= FFTW_BACKWARD;

  int flags = FFTW_UNALIGNED;
  if (optimized)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  fftwf_complex* in = new fftwf_complex[n_x*n_y*2];
  fftwf_complex* out = new fftwf_complex[n_x*n_y*2];

  plan = fftwf_plan_dft_2d (n_x, n_y, in, out, direction_flags, flags);
  nx = n_x;
  ny = n_y;
  call = fft_call;

  delete [] in;
  delete [] out;
}

FTransform::FFTW3_Plan2::~FFTW3_Plan2 ()
{
  fftwf_destroy_plan ((fftwf_plan)plan);
}

void FTransform::FFTW3_Plan2::fcc2d (unsigned nx, unsigned ny,
				     float* dest, const float* src)
{
  FFTW3_Plan2* plan = dynamic_cast<FFTW3_Plan2*>(last_fcc2d_plan);

  if (!plan || plan->nx != nx || plan->ny != ny || plan->call != "fcc2d")
    last_fcc2d_plan = plan = my_agent.get_plan (nx, ny, "fcc2d");

  fftwf_execute_dft ((fftwf_plan)(plan->plan),
		     (fftwf_complex*)src, (fftwf_complex*)dest);
}


void FTransform::FFTW3_Plan2::bcc2d (unsigned nx, unsigned ny,
				     float* dest, const float* src)
{
  FFTW3_Plan2* plan = dynamic_cast<FFTW3_Plan2*>(last_bcc2d_plan);

  if (!plan || plan->nx != nx || plan->ny != ny || plan->call != "bcc2d")
    last_bcc2d_plan = plan = my_agent.get_plan (nx, ny, "bcc2d");

  fftwf_execute_dft ((fftwf_plan)(plan->plan),
		     (fftwf_complex*)src, (fftwf_complex*)dest);
}


#endif
