/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

#include "Error.h"

#include <stdlib.h>

using namespace std;

bool FTransform::optimize = false;

// ////////////////////////////////////////////////////////////////////
//
// Global variables for one-dimensional FFT library interface
//
// ////////////////////////////////////////////////////////////////////

FTransform::fft_call FTransform::frc1d = 0;
FTransform::fft_call FTransform::fcc1d = 0;
FTransform::fft_call FTransform::bcc1d = 0;
FTransform::fft_call FTransform::bcr1d = 0;

FTransform::Plan* FTransform::last_frc1d = 0;
FTransform::Plan* FTransform::last_fcc1d = 0;
FTransform::Plan* FTransform::last_bcc1d = 0;
FTransform::Plan* FTransform::last_bcr1d = 0;

std::vector< Reference::To<FTransform::Agent> > FTransform::Agent::libraries;

// ////////////////////////////////////////////////////////////////////
//
// Global variables for two-dimensional FFT library interface
//
// ////////////////////////////////////////////////////////////////////

FTransform::fft2_call FTransform::fcc2d = 0;
FTransform::fft2_call FTransform::bcc2d = 0;

FTransform::Plan2* FTransform::last_fcc2d = 0;
FTransform::Plan2* FTransform::last_bcc2d = 0;

std::vector< Reference::To<FTransform::Agent2> > FTransform::Agent2::libraries;


// ////////////////////////////////////////////////////////////////////
//
// Internal implementation initialization 
//
// ////////////////////////////////////////////////////////////////////

static int initialise()
{
#ifdef HAVE_MKL
  FTransform::MKL_Plan::Agent::enlist ();
#endif

#ifdef HAVE_FFTW3
  FTransform::FFTW3_Plan::Agent::enlist ();
#else
#ifdef HAVE_FFTW
  FTransform::FFTW_Plan::Agent::enlist ();
#endif
#endif

#ifdef HAVE_IPP
  FTransform::IPP_Plan::Agent::enlist ();
#endif

  if (FTransform::Agent::get_num_libraries() == 0) {
    cerr << "\nFTransform: ERROR No FFT libraries installed!\n\n";
    exit(-1);
  }

  return 0;
}

static int initialised = initialise();

static FTransform::norm_type current_norm  = FTransform::normal;
static FTransform::Agent*    current_agent = 0;

void FTransform::Agent::install ()
{
  FTransform::frc1d = this->frc1d;
  FTransform::fcc1d = this->fcc1d;
  FTransform::bcc1d = this->bcc1d;
  FTransform::bcr1d = this->bcr1d;

  current_norm = this->norm;
  current_agent = this;
}

void FTransform::Agent::add ()
{ 
  libraries.push_back (this);
  if (!FTransform::fcc1d)
    install ();
}

//! Returns currently selected library
string FTransform::get_library()
{
  return current_agent->name;
}

//! Returns currently selected normalization
FTransform::norm_type FTransform::get_norm()
{
  return current_norm;
}

//! Clears out the memory associated with the plans
void FTransform::clean_plans()
{
  for (unsigned ilib=0; ilib < FTransform::Agent::libraries.size(); ilib++)
    FTransform::Agent::libraries[ilib]->clean_plans ();

  last_frc1d = 0;
  last_fcc1d = 0;
  last_bcc1d = 0;
  last_bcr1d = 0;
}

//! Choose to use a different library
void FTransform::set_library (const string& name)
{
  for (unsigned ilib=0; ilib<FTransform::Agent::libraries.size(); ilib++){
    if (FTransform::Agent::libraries[ilib]->name == name){
      FTransform::Agent::libraries[ilib]->install ();
      return;
    }
  }

  string s;

  for (unsigned ilib=0; ilib<FTransform::Agent::get_num_libraries(); ilib++)
    s += "'" + FTransform::Agent::libraries[ilib]->name + "' ";
    
  throw Error (InvalidState, "FTransform::set_library",
	       "Library '" + name + "' is not in valid libraries: " + s);
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_frc1d (unsigned ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat+2 )
    dest.resize( ndat+2 );

  float* pdest = &*dest.begin();

  frc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,(ndat+2)*sizeof(float));

  return 0;
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_fcc1d (unsigned ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  fcc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));

  return 0;
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_bcc1d (unsigned ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  bcc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));

  return 0;
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_bcr1d (unsigned ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  bcr1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));

  return 0;
}

