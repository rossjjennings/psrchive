/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo - 2015 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FortranSNR.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include <math.h>
#include <config.h>

using namespace std;
using namespace Pulsar;

#define F77_smooth_mw F77_FUNC_(smooth_mw,SMOOTH_MW)

extern "C" void F77_smooth_mw(float* period, int* nbin, int* maxw, float* rms,
                               int * kwmax, float * snrmax, float * smmax,
                               float * workspace);

#define F77_smooth_mmw F77_FUNC_(smooth_mmw,SMOOTH_MMW)

extern "C" void F77_smooth_mmw(float* period, int* nbin, int* minw, int* maxw, 
			       float* rms,
                               int * kwmax, float * snrmax, float * smmax,
                               float * workspace);

FortranSNR::FortranSNR() 
{
  minwidthbins = 0;
  maxwidthbins = 0;
  rms = 0;
  rms_set = false;
  bestwidth = 0;
}

float FortranSNR::get_snr (const Profile* profile)
{
  int nb = profile->get_nbin();
  int kwmax;
  int maxw;
  if (maxwidthbins != 0){
    maxw = maxwidthbins;
  }
  else
    maxw = nb/2;

  float snrmax,smmax;

  work.resize (nb);

  float* amps = const_cast<float*>( profile->get_amps() );

  if (!rms_set)
    rms = profile->baseline()->get_rms();

  if (minwidthbins == 0)
  {
    F77_smooth_mw(amps,&nb,&maxw,&rms,&kwmax,&snrmax,&smmax,&(work[0]));
    set_bestwidth(kwmax);
  }
  else
  {
    F77_smooth_mmw(amps,&nb,&minwidthbins,&maxw,&rms,&kwmax,&snrmax,&smmax,&(work[0]));
    set_bestwidth(kwmax);
  }

  return snrmax;
}

Phase::Value Pulsar::FortranSNR::get_width () const
{
  return Phase::Value (bestwidth, Phase::Bins);
}


class Pulsar::FortranSNR::Interface
  : public TextInterface::To<FortranSNR>
{
public:
  Interface (FortranSNR* instance)
  {
    if (instance)
      set_instance (instance);

    add( &FortranSNR::get_width,
         "width", "width of the on-pulse window that maximized S/N" );

  }

  std::string get_interface_name () const { return "pdmp"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::FortranSNR::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::FortranSNR* Pulsar::FortranSNR::clone () const
{
  return new FortranSNR (*this);
}
