/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/ObservationUncertainty.h"
#include "Pauli.h"

using namespace std;

//! Given a coherency matrix, return the difference
double Calibration::ObservationUncertainty::get_weighted_norm
(const Jones<double>& matrix) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );
  double difference = 0.0;

  for (unsigned ipol=0; ipol<4; ipol++)
    difference += norm(stokes[ipol]) * inv_variance[ipol];

  return difference;
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::ObservationUncertainty::get_weighted_conjugate
(const Jones<double>& matrix) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] = complex<double>(inv_variance[ipol]) * conj(stokes[ipol]);

  return convert (stokes);
}

//! Set the uncertainty of the observation
void Calibration::ObservationUncertainty::set_variance
(const Stokes<double>& variance)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_variance[ipol] = 1.0 / variance[ipol];
}

Calibration::ObservationUncertainty::ObservationUncertainty
(const Stokes<double>& variance)
{
  set_variance( variance );
}
