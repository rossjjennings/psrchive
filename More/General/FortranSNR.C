/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FortranSNR.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <config.h>

using namespace std;
using namespace Pulsar;

#define F77_smooth_mw F77_FUNC_(smooth_mw,SMOOTH_MW)

extern "C" void F77_smooth_mw(float* period, int* nbin, int* maxw, float* rms,
                               int * kwmax, float * snrmax, float * smmax,
                               float * workspace);

FortranSNR::FortranSNR() {}

float FortranSNR::get_snr (const Profile* profile)
{
  int nb = profile->get_nbin();
  int kwmax;
  int maxw = nb/2;
  float snrmax,smmax;

  float * workspace = new float[nb];

  float* amps = const_cast<float*>( profile->get_amps() );

  F77_smooth_mw(amps,&nb,&maxw,&rms,&kwmax,&snrmax,&smmax,workspace);
  delete [] workspace;

  return snrmax;
}



