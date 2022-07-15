/***************************************************************************
 *
 *   Copyright (C) 2006 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorPolicy.h"
#include "templates.h"

#include <iostream>
#include <cassert>

using namespace std;

using Pulsar::FluxCalibrator;

FluxCalibrator::VariableGain::VariableGain ()
  : unity(1.0)
{
  flux_cal = unity/(unity/ratio_on - unity/ratio_off);
  flux_sys = flux_cal / ratio_off;
}

FluxCalibrator::VariableGain* FluxCalibrator::VariableGain::clone () const
{
  return new VariableGain;
}

void FluxCalibrator::VariableGain::integrate (Signal::Source source,
					      unsigned ireceptor,
					      const Estimate<double>& hi,
					      const Estimate<double>& lo)
{
  Estimate<double> unity(1.0);

  // Take the ratio of the flux
  Estimate<double> ratio = hi/lo - unity ;
  if (source == Signal::FluxCalOn)
    add_ratio_on (ireceptor, ratio);
  else if (source == Signal::FluxCalOff)
    add_ratio_off (ireceptor, ratio);
}

//! Add to the mean hi/lo ratio on source for the specified receptor
void FluxCalibrator::VariableGain::add_ratio_on (unsigned ir,
						 Estimate<double>& ratio)
{
  // cerr << "FluxCalibrator::VariableGain::add_ratio_on nreceptor=" << get_nreceptor() << endl;
  assert (ir < get_nreceptor());
  mean_ratio_on.resize( get_nreceptor() );
  mean_ratio_on[ir] += ratio;
  calculated = false;
}

//! Add to the mean hi/lo ratio on source for the specified receptor
void FluxCalibrator::VariableGain::add_ratio_off (unsigned ir,
						  Estimate<double>& ratio)
{
  assert (ir < get_nreceptor());
  mean_ratio_off.resize( get_nreceptor() );
  mean_ratio_off[ir] += ratio;
  calculated = false;
}

void FluxCalibrator::VariableGain::compute (unsigned ireceptor,
					    Estimate<double>& S_cal,
					    Estimate<double>& S_sys)
{
  if (calculated)
    return;
 
  if (mean_ratio_on.size() <= ireceptor)
    throw Error (InvalidState, "FluxCalibrator::VariableGain::calculate",
		 "no on-source observations available"
                 " (on.size=%u ircptr=%u)", mean_ratio_on.size(), ireceptor);

  if (mean_ratio_off.size() <= ireceptor)
    throw Error (InvalidState, "FluxCalibrator::VariableGain::calculate",
		 "no off-source observations available"
                 " (off.size=%u ircptr=%u)", mean_ratio_off.size(), ireceptor);

  // the flux density of the standard candle in each polarization
  double S_std_i = S_std / 2;

  valid = true;

  Estimate<double> on  = mean_ratio_on[ireceptor].get_Estimate();
  Estimate<double> off = mean_ratio_off[ireceptor].get_Estimate();

  if (on==0 || off==0)
  {
    S_cal = S_sys = 0;
    valid = false;
    return;
  }

  ratio_on.set_value (on);
  ratio_off.set_value (off);

  S_sys = S_std_i * flux_sys.get_Estimate();
  S_cal = S_std_i * flux_cal.get_Estimate();

  if (S_cal.val < S_cal.get_error() ||
      S_sys.val < S_sys.get_error() )
  {
    S_cal = S_sys = 0;
    valid = false;
  }
}

