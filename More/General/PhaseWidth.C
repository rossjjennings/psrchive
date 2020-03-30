/***************************************************************************
 *
 *   Copyright (C) 2003 - 2019 by Aidan Hotan and Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWidth.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/BaselineWindow.h"
#include "Pulsar/Smooth.h"

#include <math.h>

#define _DEBUG 0
#include <iostream>

using namespace std;

Pulsar::PhaseWidth::PhaseWidth ()
{
  error = 0;
  fraction_of_maximum = 0.5;
  threshold_above_noise = 5.0;                // sigma

  BaselineWindow* mean = new BaselineWindow;
  mean->get_smooth()->set_turns ( 0.15 );    // DEFAULT DUTY CYCLE
  mean->set_find_minimum ();

  set_baseline_estimator( mean );
}


void Pulsar::PhaseWidth::set_fraction_of_maximum (float x)
{
  if (!(x>0 && x<1))
    throw Error (InvalidParam, "Pulsar::PhaseWidth::set_fraction_of_maximum",
		 "fraction=%f must be >0 and <1", x);
  fraction_of_maximum = x;
}

/* Return the fractional bin at which the value in amps would be
   "level", as found by linear interpolation of the values in bins b1
   and b2 */
double interpolate (int b1, int b2, double level, unsigned nbin, const float* amps)
{
  double x1 = b1;
  double x2 = b2;

  double y1 = amps[(b1+nbin)%nbin];
  double y2 = amps[(b2+nbin)%nbin];

  double slope = (y2 - y1) / (x2 - x1);
  return x1 + (level-y1)/slope;
}

double Pulsar::PhaseWidth::get_width_turns (const Profile* profile)
{
  get_baseline_estimator()->set_Profile (profile);
  get_baseline_estimator()->get_weight (&baseline);

  double baseline_mean = baseline.get_mean().get_value();
  double baseline_var  = baseline.get_variance().get_value();

  float stdev = sqrt(baseline_var);

  double max = profile->max() - baseline_mean;
  double min = profile->min() - baseline_mean;

  double level = 0.0;
  int peak_bin = -1;

  if ( fabs(max) > fabs(min) )
  {
    level = max * fraction_of_maximum;
    peak_bin = profile->find_max_bin();
  }
  else
  {
    level = min * fraction_of_maximum;
    peak_bin = profile->find_min_bin();
  }

  unsigned ntries = 3;
  std::vector<double> results (ntries);

  // three tries; on each try, level is incremented by 1 standard deviation
  level -= stdev;

  unsigned nbin = profile->get_nbin();
  const float* amps = profile->get_amps();

  for (unsigned tries = 0; tries < ntries; tries++)
  {
    if ( fabs(level) < threshold_above_noise*stdev)
    {
#if _DEBUG
      cerr << "width: level=" << level << " less than " << threshold_above_noise << " sigma=" << stdev << " from baseline mean=" << baseline_mean << endl;
#endif
      return 0.0;
    }

    int bc = peak_bin;
    while ( fabs(amps[(nbin+bc)%nbin]-baseline_mean) > fabs(level))
      bc --;

    double lo_edge = interpolate (bc, bc+1, level, nbin, amps);

    bc = peak_bin;
    while ( fabs(amps[(nbin+bc)%nbin]-baseline_mean) > fabs(level))
      bc ++;

    double hi_edge = interpolate (bc-1, bc, level, nbin, amps);

    double bin_dist = hi_edge - lo_edge;

#if _DEBUG
    cerr << "width: " << tries << " hi_edge=" << hi_edge << " lo_edge=" << lo_edge 
         << " bin_dist=" << bin_dist << endl;
#endif

    if (bin_dist < 0)
      bin_dist += float(nbin);

    results[tries] = bin_dist / float(nbin);
    level += stdev;
  }

  error = fabs(results.front()-results.back())/2.0;
  return results[1];
}

class Pulsar::PhaseWidth::Interface
  : public TextInterface::To<PhaseWidth>
{
public:
  Interface (PhaseWidth* instance)
  {
    if (instance)
      set_instance (instance);

    add( &PhaseWidth::get_fraction_of_maximum,
	 &PhaseWidth::set_fraction_of_maximum,
	 "fmax", "Fraction of maximum" );

    add( &HasBaselineEstimator::get_baseline_estimator,
         &HasBaselineEstimator::set_baseline_estimator,
         &ProfileWeightFunction::get_interface,
         "off", "Off-pulse estimator" );

    add( &PhaseWidth::get_error,
	 "error", "Error in last width estimate" );
  }

  std::string get_interface_name () const { return "transitions"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PhaseWidth::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::PhaseWidth* Pulsar::PhaseWidth::clone () const
{
  return new PhaseWidth (*this);
}

