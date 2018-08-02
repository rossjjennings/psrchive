/***************************************************************************
 *
 *   Copyright (C) 2006 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorPolicy.h"
#include "templates.h"

#include <iostream>
using namespace std;

using Pulsar::FluxCalibrator;

FluxCalibrator::ConstantGain::ConstantGain ()
  : unity(1.0)
{
  flux_cal = unity/(unity/ratio_hi - unity/ratio_lo);
  flux_sys = unity/ratio_lo;
}

FluxCalibrator::ConstantGain* FluxCalibrator::ConstantGain::clone () const
{
  // cerr << "FluxCalibrator::ConstantGain::clone" << endl;
  return new ConstantGain;
}

void FluxCalibrator::ConstantGain::integrate (Signal::Source source,
					      unsigned ireceptor,
					      const Estimate<double>& hi,
					      const Estimate<double>& lo)
{
  // cerr << "FluxCalibrator::ConstantGain::integrate" << endl;
    
  if (source == Signal::FluxCalOn)
  {
    mean_hi_on.resize( get_nreceptor() );
    mean_hi_on[ireceptor] += hi;

    mean_lo_on.resize( get_nreceptor() );
    mean_lo_on[ireceptor] += lo;
  }
  
  else if (source == Signal::FluxCalOff)
  {
    mean_hi_off.resize( get_nreceptor() );
    mean_hi_off[ireceptor] += hi;

    mean_lo_off.resize( get_nreceptor() );
    mean_lo_off[ireceptor] += lo;
  }
}

void FluxCalibrator::ConstantGain::compute (unsigned ireceptor,
					    Estimate<double>& S_cal,
					    Estimate<double>& S_sys)
{
  // cerr << "FluxCalibrator::ConstantGain::integrate" << endl;

  if (mean_hi_on.size() <= ireceptor)
    throw Error (InvalidState, "FluxCalibrator::ConstantGain::calculate",
		 "no on-source observations available");

  if (mean_hi_off.size() <= ireceptor)
    throw Error (InvalidState, "FluxCalibrator::ConstantGain::calculate",
		 "no off-source observations available");

  // the flux density of the standard candle in each polarization
  double S_std_i = S_std / 2;

  valid = true;

  Estimate<double> hi_on  = mean_hi_on[ireceptor].get_Estimate();
  Estimate<double> hi_off = mean_hi_off[ireceptor].get_Estimate();

  Estimate<double> lo_on  = mean_lo_on[ireceptor].get_Estimate();
  Estimate<double> lo_off = mean_lo_off[ireceptor].get_Estimate();

  if (hi_on==0 || hi_off==0 || lo_on==0 || lo_off==0)
  {
    S_cal = S_sys = 0;
    valid = false;
    return;
  }

  Estimate<double> unity(1.0);
  ratio_hi.set_value (hi_on/hi_off - unity);
  ratio_lo.set_value (lo_on/lo_off - unity);

  S_sys = S_std_i * flux_sys.get_Estimate();
  S_cal = S_std_i * flux_cal.get_Estimate();

  if (S_cal.val < S_cal.get_error() ||
      S_sys.val < S_sys.get_error() )
  {
    S_cal = S_sys = 0;
    valid = false;
  }
}


