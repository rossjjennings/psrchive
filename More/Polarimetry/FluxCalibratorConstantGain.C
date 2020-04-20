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
  flux_cal = unity/ratio_hi - unity/ratio_lo;
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
    mean_hi_on.resize( get_nreceptor(), 0.0 );
    mean_hi_on[ireceptor] += hi;

    mean_lo_on.resize( get_nreceptor(), 0.0 );
    mean_lo_on[ireceptor] += lo;
  }
  
  else if (source == Signal::FluxCalOff)
  {
    mean_hi_off.resize( get_nreceptor(), 0.0 );
    mean_hi_off[ireceptor] += hi;

    mean_lo_off.resize( get_nreceptor(), 0.0 );
    mean_lo_off[ireceptor] += lo;
  }
}

//! Set the number of receptors
void Pulsar::FluxCalibrator::ConstantGain::set_nreceptor (unsigned nreceptor)
{
  Policy::set_nreceptor( nreceptor );

  scale.resize( nreceptor, 0.0 );
  gain_ratio.resize( nreceptor, 0.0 );
}

void FluxCalibrator::ConstantGain::invalidate (unsigned ireceptor)
{
  // cerr << "FluxCalibrator::ConstantGain::invalidate ireceptor=" << ireceptor << endl;

  valid = false;

  scale.resize( get_nreceptor(), 0.0 );
  scale.at(ireceptor) = 0.0;

  gain_ratio.resize( get_nreceptor(), 0.0 );
  gain_ratio.at(ireceptor) = 0.0;
}

void FluxCalibrator::ConstantGain::compute (unsigned ireceptor,
					    Estimate<double>& S_cal,
					    Estimate<double>& S_sys)
{
  // cerr << "FluxCalibrator::ConstantGain::compute ireceptor=" << ireceptor << endl;

  if (mean_hi_on.size() <= ireceptor)
    throw Error (InvalidState, "FluxCalibrator::ConstantGain::compute",
		 "no on-source observations available");

  if (mean_hi_off.size() <= ireceptor)
    throw Error (InvalidState, "FluxCalibrator::ConstantGain::compute",
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
    invalidate (ireceptor);
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
    invalidate (ireceptor);
    return;
  }
  
  scale.resize( get_nreceptor(), 0.0 );
  scale[ireceptor] = (lo_on - lo_off) / S_std_i; 

  gain_ratio.resize( get_nreceptor(), 0.0 );
  gain_ratio[ireceptor] = (hi_on - hi_off) / (lo_on - lo_off);

  // cerr << "scale=" << scale[ireceptor] << endl;
}


Estimate<double> 
FluxCalibrator::ConstantGain::get_gain_ratio (unsigned ir) const
{
  return gain_ratio.at(ir);
}

void FluxCalibrator::ConstantGain::get_gain_ratio (vector<Estimate<double> >& r)
{
  r = gain_ratio;
}

void FluxCalibrator::ConstantGain::set_gain_ratio (unsigned ir,
						   const Estimate<double>& r)
{
  gain_ratio.at(ir) = r;
}

void FluxCalibrator::ConstantGain::set_gain_ratio (const vector<Estimate<double> >& r)
{
  gain_ratio = r;
}


Estimate<double> 
FluxCalibrator::ConstantGain::get_scale (unsigned ireceptor) const
{
  return scale.at(ireceptor);
}

Estimate<double> scale_to_gain (const Estimate<double>& scale)
{
  if (scale.val == 0.0)
    return 0.0;
  else
    return 1.0 / scale;
}

Estimate<double>
FluxCalibrator::ConstantGain::get_gain (unsigned ireceptor) const
{
  return scale_to_gain( get_scale (ireceptor) );
}

void FluxCalibrator::ConstantGain::get_scale (vector<Estimate<double> >& s)
{
  s = scale;
}

void FluxCalibrator::ConstantGain::set_scale (unsigned ir,
					      const Estimate<double>& s)
{
  scale.at(ir) = s;
}

void FluxCalibrator::ConstantGain::set_scale (const vector<Estimate<double> >& s)
{
  scale = s;
}


Estimate<double> FluxCalibrator::ConstantGain::get_scale () const
{
  if (scale.size() == 1)
    return scale[0];

  else if (scale.size() == 2)
    return sqrt( scale[0] * scale[1] );
  
  else
    throw Error (InvalidState, "FluxCalibrator::ConstantGain::get_gain",
		 "invalid nreceptor=%u", scale.size());
}

Estimate<double> FluxCalibrator::ConstantGain::get_gain () const
{
  return scale_to_gain( get_scale () );
}

