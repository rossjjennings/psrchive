/***************************************************************************
 *
 *   Copyright (C) 2006 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorPolicy.h"
#include "templates.h"

// #define _DEBUG 1
#include "debug.h"

using Pulsar::FluxCalibrator;

Pulsar::FluxCalibrator::Policy::Policy ()
{
  calculated = false;
  valid = true;
}

void FluxCalibrator::Policy::set ( const std::vector< Estimate<double> >& s,
				   const std::vector< Estimate<double> >& c )
{
  S_sys = s;
  S_cal = c;
  calculated = true;
  valid = true;
}

void FluxCalibrator::Policy::get ( std::vector< Estimate<double> >& s,
				   std::vector< Estimate<double> >& c ) const
try
{
  calculate();
  s = S_sys;
  c = S_cal;
}
catch (Error& error)
{
  s = S_sys;
  c = S_cal;
  throw error += "Pulsar::FluxCalibrator::Policy::get";
}

//! Set the number of receptors
void Pulsar::FluxCalibrator::Policy::set_nreceptor (unsigned nreceptor)
{
  S_sys.resize( nreceptor );
  S_cal.resize( nreceptor );
}

//! Get the number of receptors
unsigned Pulsar::FluxCalibrator::Policy::get_nreceptor () const
{
  return S_sys.size();
}

bool Pulsar::FluxCalibrator::Policy::get_solution_available () const try
{
  if (!valid)
    return false;

  if (!calculated)
    calculate ();

  return true;
}
catch (Error& error)
{
  DEBUG("FluxCalibrator::Policy::get_solution_available " << error.get_message ());
  valid = false;
  return false;
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
Estimate<double> Pulsar::FluxCalibrator::Policy::get_S_sys () const
{
  if (!valid)
    return 0;
  calculate ();
  return sum (S_sys);
}

//! Return the total calibrator flux density
Estimate<double> Pulsar::FluxCalibrator::Policy::get_S_cal () const
{
  if (!valid)
    return 0;
  calculate ();
  return sum (S_cal);
}

//! Return the system equivalent flux density in the specified receptor
Estimate<double> Pulsar::FluxCalibrator::Policy::get_S_sys (unsigned ir) const
{
  if (!valid)
    return 0;
  calculate ();
  return S_sys[ir];
}

//! Return the calibrator flux density in the specified receptor
Estimate<double> Pulsar::FluxCalibrator::Policy::get_S_cal (unsigned ir) const
{
  if (!valid)
    return 0;
  calculate ();
  return S_cal[ir];
}

//! Set the flux of the standard candle
void Pulsar::FluxCalibrator::Policy::set_S_std (double S)
{
  S_std = S;
  calculated = false;
}

//! Get the flux of the standard candle
double Pulsar::FluxCalibrator::Policy::get_S_std () const
{
  return S_std;
}

void Pulsar::FluxCalibrator::Policy::calculate () const 
{
  if (calculated)
    return;

  unsigned ir, nreceptor = get_nreceptor();

  for (ir=0; ir<nreceptor; ir++)
    S_cal[ir] = S_sys[ir] = 0;

  for (ir=0; ir<nreceptor; ir++)
    const_cast<Policy*>(this)->compute (ir, S_cal[ir], S_sys[ir]);

  calculated = true;
}

