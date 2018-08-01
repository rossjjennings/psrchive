/***************************************************************************
 *
 *   Copyright (C) 2006 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorData.h"
#include "templates.h"

#include <iostream>
using namespace std;

Pulsar::FluxCalibrator::Data::Data ()
{
  calculated = false;
  valid = true;
}

//! Construct from known flux densities
Pulsar::FluxCalibrator::Data::Data ( const std::vector< Estimate<double> >& s,
				     const std::vector< Estimate<double> >& c )
{
  S_sys = s;
  S_cal = c;
  calculated = true;
  valid = true;
}

void 
Pulsar::FluxCalibrator::Data::get ( std::vector< Estimate<double> >& s,
				    std::vector< Estimate<double> >& c ) const
try {
  calculate();
  s = S_sys;
  c = S_cal;
}
catch (Error& error) {
  s = S_sys;
  c = S_cal;
  throw error += "Pulsar::FluxCalibrator::Data::get";
}

//! Set the number of receptors
void Pulsar::FluxCalibrator::Data::set_nreceptor (unsigned nreceptor)
{
  S_sys.resize( nreceptor );
  S_cal.resize( nreceptor );
}

//! Get the number of receptors
unsigned Pulsar::FluxCalibrator::Data::get_nreceptor () const
{
  return S_sys.size();
}

//! Return the sum of all elements in a container
template< typename T, template<typename> class C >
T sum (const C<T>& container)
{
  typename C<T>::const_iterator i = container.begin();
  T ret = *i;
  for (i++; i != container.end(); i++)
    ret += *i;
  return ret;
}

//! Return the total system equivalent flux density
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_sys () const
{
  if (!valid)
    return 0;
  calculate ();
  return sum (S_sys);
}

//! Return the total calibrator flux density
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_cal () const
{
  if (!valid)
    return 0;
  calculate ();
  return sum (S_cal);
}

//! Return the system equivalent flux density in the specified receptor
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_sys (unsigned ir) const
{
  if (!valid)
    return 0;
  calculate ();
  return S_sys[ir];
}

//! Return the calibrator flux density in the specified receptor
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_cal (unsigned ir) const
{
  if (!valid)
    return 0;
  calculate ();
  return S_cal[ir];
}




//! Set the flux of the standard candle
void Pulsar::FluxCalibrator::Data::set_S_std (double S)
{
  S_std = S;
  calculated = false;
}

//! Get the flux of the standard candle
double Pulsar::FluxCalibrator::Data::get_S_std () const
{
  return S_std;
}

void Pulsar::FluxCalibrator::Data::calculate () const 
{
  if (calculated)
    return;

  unsigned ir, nreceptor = get_nreceptor();

  for (ir=0; ir<nreceptor; ir++)
    S_cal[ir] = S_sys[ir] = 0;

  for (ir=0; ir<nreceptor; ir++)
    const_cast<Data*>(this)->compute (ir, S_cal[ir], S_sys[ir]);

  calculated = true;
}

using Pulsar::FluxCalibrator;

FluxCalibrator::VariableGain
::VariableGain (const std::vector< Estimate<double> >& S,
		const std::vector< Estimate<double> >& C)
  : Data (S, C),
    unity(1.0)
{
  init ();
}

FluxCalibrator::VariableGain::VariableGain ()
  : unity(1.0)
{
  init ();
}

void FluxCalibrator::VariableGain::init ()
{
  flux_cal = unity/(unity/ratio_on - unity/ratio_off);
  flux_sys = flux_cal / ratio_off;
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
  mean_ratio_on.resize( get_nreceptor() );
  mean_ratio_on[ir] += ratio;
  calculated = false;
}

//! Add to the mean hi/lo ratio on source for the specified receptor
void FluxCalibrator::VariableGain::add_ratio_off (unsigned ir,
						  Estimate<double>& ratio)
{
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
    throw Error (InvalidState, "Pulsar::FluxCalibrator::Data::calculate",
		 "no on-source observations available");

  if (mean_ratio_off.size() <= ireceptor)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::Data::calculate",
		 "no off-source observations available");

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


