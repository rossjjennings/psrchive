#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include "psr_cpp.h"
#include "Error.h"

#include "FTransform.h"

#ifdef HAVE_MKL
#include "MKL_Transform.h"
#endif

#ifdef HAVE_FFTW3
#include "FFTW3_Transform.h"
#else
#ifdef HAVE_FFTW
#include "FFTW_Transform.h"
#endif
#endif

#ifdef HAVE_IPP
#include "IPP_Transform.h"
#endif

FTransform::fft_call FTransform::frc1d = 0;
FTransform::fft_call FTransform::fcc1d = 0;
FTransform::fft_call FTransform::bcc1d = 0;

bool FTransform::optimize = false;

FTransform::norm_type FTransform::norm = FTransform::normal;
string FTransform::library = string();
vector<string> FTransform::valid_libraries;
vector<FTransform::fft_call> FTransform::frc1d_calls;
vector<FTransform::fft_call> FTransform::fcc1d_calls;
vector<FTransform::fft_call> FTransform::bcc1d_calls;
vector<FTransform::norm_type> FTransform::norms;
vector<vector<Reference::To<FTransform::Plan> > > FTransform::plans;
FTransform::Plan* FTransform::last_frc1d_plan = 0;
FTransform::Plan* FTransform::last_fcc1d_plan = 0;
FTransform::Plan* FTransform::last_bcc1d_plan = 0;

int FTransform::initialised = FTransform::initialise();

int FTransform::initialise(){
  fprintf(stderr,"In FTransform::initialise()\n");

  int ret = -1;

#ifdef HAVE_MKL
  ret =   mkl_initialise();
#endif
#ifdef HAVE_FFTW3
  ret = fftw3_initialise();
#else
#ifdef HAVE_FFTW
  ret =  fftw_initialise();
#endif
#endif
#ifdef HAVE_IPP
  ret =   ipp_initialise();
#endif

  if( ret < 0 ){
    fprintf(stderr,"\nFTransform::initialise(): ERROR: No FFT libraries installed!\n\n");
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
unsigned FTransform::get_ilib(string libstring){
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
void FTransform::set_library(string _library){
  library = _library;

  unsigned ilib = get_ilib();

  norm = norms[ilib];

  frc1d = frc1d_calls[ilib];
  fcc1d = fcc1d_calls[ilib];
  bcc1d = bcc1d_calls[ilib];
}

FTransform::Plan::Plan() : Reference::Able() {
  optimized = optimize;
  ndat = 0;
  ilib = 0;
}

FTransform::Plan::Plan(unsigned _ndat, unsigned _ilib, string _fft_call)
  : Reference::Able() 
{
  initialise(_ndat,_ilib,_fft_call);
}

void FTransform::Plan::initialise(unsigned _ndat, unsigned _ilib, string _fft_call){
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
