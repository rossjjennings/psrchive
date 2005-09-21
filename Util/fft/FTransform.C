#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "FTransform.h"

#ifdef HAVE_MKL
#include "MKL_Transform.h"
#endif

#ifdef HAVE_FFTW3
#include "FFTW3_Transform.h"
#endif

#ifdef HAVE_FFTW
#include "FFTW_Transform.h"
#endif

#ifdef HAVE_IPP
#include "IPP_Transform.h"
#endif

#include "Error.h"

#include <string>
#include <vector>

#include <stdlib.h>

using namespace std;

FTransform::fft_call FTransform::frc1d = 0;
FTransform::fft_call FTransform::fcc1d = 0;
FTransform::fft_call FTransform::bcc1d = 0;
FTransform::fft_call FTransform::bcr1d = 0;

bool FTransform::optimize = false;

FTransform::norm_type FTransform::norm = FTransform::normal;
string FTransform::library = string();
vector<string> FTransform::valid_libraries;
vector<FTransform::fft_call> FTransform::frc1d_calls;
vector<FTransform::fft_call> FTransform::fcc1d_calls;
vector<FTransform::fft_call> FTransform::bcc1d_calls;
vector<FTransform::fft_call> FTransform::bcr1d_calls;
vector<FTransform::norm_type> FTransform::norms;
vector<vector<Reference::To<FTransform::Plan> > > FTransform::plans;
FTransform::Plan* FTransform::last_frc1d_plan = 0;
FTransform::Plan* FTransform::last_fcc1d_plan = 0;
FTransform::Plan* FTransform::last_bcc1d_plan = 0;
FTransform::Plan* FTransform::last_bcr1d_plan = 0;

// ////////////////////////////////////////////////////////////////////
//
// Global variables for two-dimensional FFT library interface
//
// ////////////////////////////////////////////////////////////////////

FTransform::fft2_call FTransform::fcc2d = 0;
FTransform::fft2_call FTransform::bcc2d = 0;

FTransform::Plan2* FTransform::last_fcc2d_plan = 0;
FTransform::Plan2* FTransform::last_bcc2d_plan = 0;

std::vector< Reference::To<FTransform::Agent2> > FTransform::Agent2::libraries;

int FTransform::initialised = FTransform::initialise();

int FTransform::initialise()
{
  int ret = -1;

#ifdef HAVE_MKL
  ret =   mkl_initialise();
#endif
#ifdef HAVE_FFTW3
  ret = fftw3_initialise();
#endif
#ifdef HAVE_FFTW
  ret =  fftw_initialise();
#endif
#ifdef HAVE_IPP
  ret =   ipp_initialise();
#endif

  if (ret < 0) {
    cerr << "\nFTransform::initialise: ERROR: No FFT libraries installed!\n\n";
    exit(-1);
  }

  plans.resize( valid_libraries.size() );

  return 234;
}

//! Returns currently selected library
string FTransform::get_library(){
  return library;
}

//! Returns currently selected normalization
FTransform::norm_type FTransform::get_norm(){
  return norm;
}

//! Clears out the memory associated with the plans
void FTransform::clean_plans(){
  plans.resize(0);
  last_frc1d_plan = 0;
  last_fcc1d_plan = 0;
  last_bcc1d_plan = 0;
  last_bcr1d_plan = 0;
}

//! Returns index of library in use
unsigned FTransform::get_ilib(){
  for( unsigned ilib=0; ilib<valid_libraries.size(); ilib++)
    if( valid_libraries[ilib] == library )
      return ilib;

  string s;

  for( unsigned ilib=0; ilib<valid_libraries.size(); ilib++)
    s += valid_libraries[ilib] + " ";
  
  throw Error(InvalidState,"FTransform::set_library()",
	      "Library '%s' is not valid- valid libraries are: %s",
	      library.c_str(), s.c_str());
  
  return 0;
}

//! Returns index of a particular library
unsigned FTransform::get_ilib (const string& libstring)
{
  for( unsigned ilib=0; ilib<valid_libraries.size(); ilib++)
    if( valid_libraries[ilib] == libstring )
      return ilib;

  string s;

  for( unsigned ilib=0; ilib<valid_libraries.size(); ilib++)
    s += valid_libraries[ilib] + " ";
    
  throw Error(InvalidState,"FTransform::set_library(string)",
	      "Library '%s' is not valid- valid libraries are: %s",
	      libstring.c_str(), s.c_str());

  return 0;
}

//! Choose to use a different library
void FTransform::set_library (const string& _library)
{
  library = _library;

  unsigned ilib = get_ilib();

  norm = norms[ilib];

  frc1d = frc1d_calls[ilib];
  fcc1d = fcc1d_calls[ilib];
  bcc1d = bcc1d_calls[ilib];
  bcr1d = bcr1d_calls[ilib];
}

FTransform::Plan::Plan() : Reference::Able() {
  optimized = optimize;
  ndat = 0;
  ilib = 0;
}

FTransform::Plan::Plan(unsigned _ndat, unsigned _ilib, const string& _fft_call)
{
  initialise(_ndat,_ilib,_fft_call);
}

void FTransform::Plan::initialise(unsigned _ndat, unsigned _ilib, 
				  const string& _fft_call){
  optimized = optimize;
  ndat = _ndat;
  ilib = _ilib;
  fft_call = _fft_call;

  if( ilib >= plans.size() )
    throw Error(InvalidState,"FTransform::Plan::initialise()",
		"input ilib (%d) is too big for number of plans stored! (%d)",
		ilib, plans.size());

  plans[ilib].push_back( this );
}

FTransform::Plan::~Plan() { }

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_frc1d(unsigned ndat, float* srcdest){
  static vector<float> dest;
  if( dest.size() != ndat+2 )
    dest.resize( ndat+2 );

  float* pdest = &*dest.begin();

  frc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,(ndat+2)*sizeof(float));
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_fcc1d(unsigned ndat, float* srcdest){
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  fcc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_bcc1d(unsigned ndat, float* srcdest){
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  bcc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_bcr1d(unsigned ndat, float* srcdest){
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  bcr1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));
}

