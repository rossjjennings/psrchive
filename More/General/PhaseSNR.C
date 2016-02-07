/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseSNR.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include <math.h>

using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

float Pulsar::PhaseSNR::get_snr (const Profile* profile)
{
  // find the mean and the r.m.s. of the baseline
  double min_avg, min_var;
  profile->stats (profile->find_min_phase(), &min_avg, &min_var);
  double min_rms = sqrt (min_var);

  if (Profile::verbose)
    cerr << "Pulsar::snr_phase rms=" << min_rms << endl;

  if (min_rms == 0.0)
    return 0;

  // find the total power under the pulse
  int rise = 0, fall = 0;
  profile->find_peak_edges (rise, fall);

  double power = profile->sum (rise, fall);

  nbinify (rise, fall, profile->get_nbin());

  if (Profile::verbose)
    cerr << "Pulsar::snr_phase rise=" << rise << " fall=" << fall 
	 << " power=" << power << endl;

  // subtract the total power due to the baseline
  power -= min_avg * double (fall - rise);

  // divide by the sqrt of the number of bins
  power /= sqrt (double(fall-rise));

  return power/min_rms;
}



class Pulsar::PhaseSNR::Interface
  : public TextInterface::To<PhaseSNR>
{
public:
  Interface (PhaseSNR* instance)
  {
    if (instance)
      set_instance (instance);
  }

  std::string get_interface_name () const { return "phase"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PhaseSNR::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::PhaseSNR* Pulsar::PhaseSNR::clone () const
{
  return new PhaseSNR (*this);
}
