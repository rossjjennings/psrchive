/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/CoherencyMeasurement.h"
#include "Pauli.h"

using namespace std;

Calibration::CoherencyMeasurement::CoherencyMeasurement (unsigned index)
{
  uncertainty = 0;
  input_index = index;
}

//! Set the index of the input to which the measurement corresponds
void Calibration::CoherencyMeasurement::set_input_index (unsigned index)
{
  input_index = index;
}

//! Set the index of the input to which the measurement corresponds
unsigned Calibration::CoherencyMeasurement::get_input_index () const
{
  return input_index;
}

//! Set the measured Stokes parameters
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< Estimate<double> >& stokes)
{
  Stokes<double> temp;
  for (unsigned ipol=0; ipol<4; ipol++) {
    temp[ipol]    = stokes[ipol].val;
    inv_var[ipol] = 1.0/stokes[ipol].var;
  }

  rho = convert (temp);
}

//! Set the measured complex Stokes parameters
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< complex<double> >& stokes, const Stokes<double>& variance)
{
  for (unsigned ipol=0; ipol<4; ipol++)
    inv_var[ipol] = 1.0/variance[ipol];

  rho = convert (stokes);
}

//! Set the measured complex Stokes parameters and the variance functions
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< complex<double> >& stokes, const Uncertainty* var)
{
  rho = convert (stokes);
  uncertainty = var;
}

//! Get the measured coherency matrix
Jones<double> Calibration::CoherencyMeasurement::get_coherency () const
{
  return rho;
}

//! Set the measured Stokes parameters
Stokes< Estimate<double> >
Calibration::CoherencyMeasurement::get_stokes () const
{
  Stokes<double> temp = coherency( rho );
  Stokes< Estimate<double> > stokes;

  for (unsigned ipol=0; ipol<4; ipol++) {
    stokes[ipol].val = temp[ipol];
    stokes[ipol].var = 1.0/get_inv_var(ipol);
  }

  return stokes;
}

float Calibration::CoherencyMeasurement::get_variance (unsigned ipol) const
{
  return 1.0/get_inv_var(ipol);
}

//! Given a coherency matrix, return the difference
double Calibration::CoherencyMeasurement::get_weighted_norm
(const Jones<double>& matrix) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );
  double difference = 0.0;

  for (unsigned ipol=0; ipol<4; ipol++)
    difference += norm(stokes[ipol]) * get_inv_var(ipol);

  return difference;
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::CoherencyMeasurement::get_weighted_conjugate
(const Jones<double>& matrix) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] = complex<double>(get_inv_var(ipol)) * conj(stokes[ipol]);

  return convert (stokes);
}

