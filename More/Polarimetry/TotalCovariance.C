/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/TotalCovariance.h"
#include "MEAL/StokesCovariance.h"

#include <iostream>
using namespace std;

//! Set the optimizing transformation
void Calibration::TotalCovariance::set_optimizing_transformation 
(const Matrix<4,4,double>& opt)
{
 optimizer = opt;
}

//! Get the total variance in the specified Stokes parameter
double Calibration::TotalCovariance::get_variance (unsigned ipol) const
{
  if (!built)
    const_cast<TotalCovariance*>(this)->build();
  return 1.0/inv_covar[ipol][ipol];
}

//! Given a coherency matrix, return the difference
double Calibration::TotalCovariance::get_weighted_norm
(const Jones<double>& matrix) const
{
  if (!built)
    const_cast<TotalCovariance*>(this)->build();

  Stokes< complex<double> > stokes = complex_coherency( matrix );
  return (stokes * inv_covar * conj(stokes)).real();
}


//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::TotalCovariance::get_weighted_conjugate
(const Jones<double>& matrix) const
{
  if (!built)
    const_cast<TotalCovariance*>(this)->build();

  Stokes< complex<double> > stokes = complex_coherency( matrix );
  Stokes< complex<double> > result = inv_covar * conj(stokes);

  return convert (result);
}

void Calibration::TotalCovariance::build ()
{
  MEAL::StokesCovariance compute;

  compute.set_variance (template_variance);
  compute.set_transformation (optimizer * Mueller(transformation->evaluate()));

  Matrix<4,4,double> covar = compute.get_covariance();

  compute.set_variance (observation_variance);
  compute.set_transformation (optimizer);

  covar += compute.get_covariance();

  inv_covar = inv (covar);
  built = true;
}
