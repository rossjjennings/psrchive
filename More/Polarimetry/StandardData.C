/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardData.h"
#include "Pulsar/PolnProfile.h"

// #define _DEBUG 1
#include "debug.h"

using namespace std;

//! Default constructor
/*! If specified, baseline and on-pulse regions are defined by select */
Calibration::StandardData::StandardData (const Pulsar::PolnProfile* select)
{
  stats = new Pulsar::PolnProfileStats;

  if (select)
    select_profile (select);
}

//! Select the baseline and on-pulse regions from profile
void Calibration::StandardData::select_profile (const Pulsar::PolnProfile* p)
{
  if (normalize)
    stats -> set_avoid_zero_determinant ();

  stats->select_profile (p);
  total_squared_invariant = stats->get_total_squared_invariant ();
  baseline = stats->get_baseline();

  cerr << "Calibration::StandardData::select_profile baseline=" << baseline << endl;
}

//! Set the profile from which estimates will be derived
void Calibration::StandardData::set_profile (const Pulsar::PolnProfile* p)
{
  stats->set_profile (p);

  DEBUG("Calibration::StandardData::set_profile onpulse nbin=" << stats->get_stats()->get_onpulse_nbin());

  total_squared_invariant = stats->get_total_squared_invariant ();
  baseline = stats->get_baseline();

  DEBUG("Calibration::StandardData::set_profile baseline=" << baseline);
}

//! Normalize estimates by the average determinant
void Calibration::StandardData::set_normalize (bool norm)
{
  DEBUG("Calibration::StandardData::set_normalize " << norm);

  if (norm)
    normalize = new MEAL::NormalizeStokes;
  else
    normalize = 0;
}

//! Get the Stokes parameters of the specified phase bin
Stokes<Estimate<double>>
Calibration::StandardData::get_stokes (unsigned ibin)
{
  Stokes<Estimate<double>> result = stats->get_stokes (ibin);

  DEBUG("Calibration::StandardData::get_stokes ibin=" << ibin << " stokes=" << result);
  DEBUG("Calibration::StandardData::get_stokes baseline=" << baseline);
  result -= baseline;

  if (normalize)
  {
    DEBUG("Calibration::StandardData::get_stokes normalize total_inv_sq=" << total_squared_invariant);
    normalize->normalize (result, total_squared_invariant);
    result *= sqrt( (double) stats->get_stats()->get_onpulse_nbin() );
  }

  DEBUG("Calibration::StandardData::get_stokes ibin=" << ibin << endl << "result=" << result);
  return result;
}

Stokes<Estimate<double>>
Calibration::StandardData::get_baseline()
{
  Stokes<Estimate<double>> result = baseline;
  if (normalize)
  {
    result /= sqrt(baseline.invariant());
  }

  return result;
}

//! Get the algorithm used to compute the profile statistics
Pulsar::PolnProfileStats* Calibration::StandardData::get_poln_stats ()
{
  return stats;
}
