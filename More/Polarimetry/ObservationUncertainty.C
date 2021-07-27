/***************************************************************************
 *
 *   Copyright (C) 2006 - 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ObservationUncertainty.h"
#include "Pauli.h"

using namespace std;
using namespace Calibration;

template<typename T> T sqr (T x) { return x*x; }

//! Given a coherency matrix, return the difference
double ObservationUncertainty::get_weighted_norm
(const Jones<double>& matrix) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );
  double difference = 0.0;
  for (unsigned ipol=0; ipol<4; ipol++)
  {
    difference += sqr(stokes[ipol].real()) * inv_variance[ipol].real();
    difference += sqr(stokes[ipol].imag()) * inv_variance[ipol].imag();
  }
  return difference;
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> ObservationUncertainty::get_weighted_conjugate
( const Jones<double>& matrix ) const try
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );

  // cerr << "inv=" << inv_variance << endl;

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] =
      complex<double>( inv_variance[ipol].real() * stokes[ipol].real(),
		      -inv_variance[ipol].imag() * stokes[ipol].imag() );

  return convert (stokes);
}
catch (Error& error)
{
  throw error += "ObservationUncertainty::get_weighted_conjugate";
}

Stokes< complex<double> >
ObservationUncertainty::get_weighted_components
( const Jones<double>& matrix ) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] =
      complex<double>( sqrt(inv_variance[ipol].real()) * stokes[ipol].real(),
		       sqrt(inv_variance[ipol].imag()) * stokes[ipol].imag() );

  return stokes;
}

//! Set the uncertainty of the observation
void ObservationUncertainty::set_variance
( const Stokes< complex<double> >& variance )
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_variance[ipol] = complex<double>( 1.0 / variance[ipol].real(),
					  1.0 / variance[ipol].imag() );
}

//! Set the uncertainty of the observation
void ObservationUncertainty::set_variance
( const Stokes<double>& variance )
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_variance[ipol] = complex<double>( 1.0 / variance[ipol],
					  1.0 / variance[ipol] );
}

//! Return the variance of each Stokes parameter
Stokes< std::complex<double> > 
ObservationUncertainty::get_variance () const
{
  Stokes< std::complex<double> > result;

  for (unsigned ipol=0; ipol < 4; ipol++)
  {
    double re = 0.0;
    if (inv_variance[ipol].real())
      re = 1.0/inv_variance[ipol].real();

    double im = 0.0;
    if (inv_variance[ipol].imag())
      im = 1.0/inv_variance[ipol].imag();

    result[ipol] = complex<double>( re, im );
  }

  return result;
}

//! Construct with the uncertainty of the observation
ObservationUncertainty::ObservationUncertainty (const Stokes<double>& var)
{
  set_variance (var);
}

//! Return a copy constructed clone of self
ObservationUncertainty* ObservationUncertainty::clone () const
{
  return new ObservationUncertainty (*this);
}

complex<double> getvar (const complex<double>& inv_var)
{
  double revar = 0.0;
  double imvar = 0.0;
  
  if (inv_var.real() != 0)
    revar = 1.0 / inv_var.real();
  if (inv_var.imag() != 0)
    imvar = 1.0 / inv_var.imag();

  return complex<double> (revar, imvar);
}
		     
//! Add the uncertainty of another instance
void ObservationUncertainty::add (const Uncertainty* other)
{
  const ObservationUncertainty* like
    = dynamic_cast<const ObservationUncertainty*> (other);
  
  if (!like)
    throw Error (InvalidParam, "ObservationUncertainty::add",
		 "other Uncertainty is not an ObservationUncertainty");

  for (unsigned ipol=0; ipol < 4; ipol++)
  {
    complex<double> var = getvar(inv_variance[ipol]);
    var += getvar(like->inv_variance[ipol]);
    
    inv_variance[ipol] = complex<double>( 1.0 / var.real(),
					  1.0 / var.imag() );
  }
}
