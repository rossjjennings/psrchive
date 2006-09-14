/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/CoherencyMeasurement.h"
#include "Calibration/ObservationUncertainty.h"
#include "Pauli.h"

using namespace std;

Calibration::CoherencyMeasurement::CoherencyMeasurement (unsigned index)
{
  nconstraint = 0;
  uncertainty = 0;
  input_index = index;
}

//! Set the index of the input to which the measurement corresponds
void Calibration::CoherencyMeasurement::set_input_index (unsigned index)
{
  input_index = index;
}

//! Get the index of the input to which the measurement corresponds
unsigned Calibration::CoherencyMeasurement::get_input_index () const
{
  return input_index;
}

//! Get the number of constraints provided by this measurement
unsigned Calibration::CoherencyMeasurement::get_nconstraint () const
{
  return nconstraint;
}

//! Set the measured Stokes parameters
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< Estimate<double> >& stokes)
{
  Stokes<double> val;

  for (unsigned ipol=0; ipol<4; ipol++) {
    val[ipol] = stokes[ipol].val;
    variance[ipol] = stokes[ipol].var;
  }

  rho = convert (val);
  uncertainty = new ObservationUncertainty (variance);

  // 4 Stokes
  nconstraint = 4;
}

//! Get the measured Stokes parameters
Stokes< Estimate<double> > 
Calibration::CoherencyMeasurement::get_stokes () const
{
  Stokes< Estimate<double> > result;
  Stokes< double > val = coherency (rho);

  for (unsigned ipol=0; ipol<4; ipol++) {
    result[ipol].val = val[ipol];
    result[ipol].var = variance[ipol];
  }

  return result;
}

//! Set the measured complex Stokes parameters
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< complex<double> >& stokes, const Stokes<double>& var)
{
  rho = convert (stokes);
  variance = var;

  uncertainty = new ObservationUncertainty (variance);

  // 4 Stokes, Re and Im
  nconstraint = 8;
}

//! Set the measured complex Stokes parameters and the variance functions
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< complex<double> >& stokes, const Uncertainty* var)
{
  uncertainty = var;

  rho = convert (stokes);
  variance = 0;

  // 4 Stokes, Re and Im
  nconstraint = 8;
}

//! Get the measured coherency matrix
Jones<double> Calibration::CoherencyMeasurement::get_coherency () const
{
  return rho;
}

//! Given a coherency matrix, return the difference
double Calibration::CoherencyMeasurement::get_weighted_norm
(const Jones<double>& matrix) const
{
  return uncertainty->get_weighted_norm (matrix);
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::CoherencyMeasurement::get_weighted_conjugate
(const Jones<double>& matrix) const
{
  return uncertainty->get_weighted_conjugate (matrix);
}

