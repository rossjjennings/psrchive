/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardData.h"
#include "Pulsar/PolnProfile.h"

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
  stats->select_profile (p);
}

//! Set the profile from which estimates will be derived
void Calibration::StandardData::set_profile (const Pulsar::PolnProfile* p)
{
  stats->set_profile (p);
  profile = p;

  for (unsigned i=0; i<4; i++)
    profile_variance[i] = stats->get_baseline_variance(i).get_value();

  total_determinant = stats->get_total_determinant ();
}

//! Normalize estimates by the average determinant
void Calibration::StandardData::set_normalize (bool norm)
{
  if (norm)
    normalize = new MEAL::NormalizeStokes;
  else
    normalize = 0;
}



//! Get the Stokes parameters of the specified phase bin
Stokes< Estimate<double> >
Calibration::StandardData::get_stokes (unsigned ibin)
{
  Stokes< Estimate<double> > result = stats->get_stokes (ibin);

  if (normalize)
  {
    Estimate<double> invint = invariant (result);

    // the total determinant, less the determinant of this phase bin
    Estimate<double> total_other = total_determinant;
    total_other.val -= invint.val;
    total_other.var -= invint.var;
    
    normalize->set_other (total_other);
    normalize->normalize (result);

    result *= sqrt( stats->get_on_pulse_nbin() );
  }

  return result;
}
