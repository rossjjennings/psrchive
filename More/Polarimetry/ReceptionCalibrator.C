#include "ReceptionCalibrator.h"

Pulsar::ReceptionCalibrator::ReceptionCalibrator ()
{
  nchan = 1;
  nsource = 1;
  ncalibrator = 0;
}

//! Set the number of source polarization states for which to solve
void Pulsar::ReceptionCalibrator::set_nsource (unsigned _nsource)
{
  nsource = _nsource;
}

//! Get the number of source polarization states for which to solve
unsigned Pulsar::ReceptionCalibrator::get_nsource () const
{
  return nsource;
}

//! Set the number of calibrator polarization states for which to solve
void Pulsar::ReceptionCalibrator::set_ncalibrator (unsigned _ncalibrator)
{
  ncalibrator = _ncalibrator;
}

//! Get the number of calibrator polarization states for which to solve
unsigned Pulsar::ReceptionCalibrator::get_ncalibrator () const
{
  return ncalibrator;
}

//! Set the number of frequency channels
void Pulsar::ReceptionCalibrator::set_nchan (unsigned _nchan)
{
  nchan = _nchan;
  model.resize (nchan);
}

//! Get the number of frequency channels
unsigned Pulsar::ReceptionCalibrator::get_nchan () const
{
  return nchan;
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::calibrate (Archive* archive)
{
  cerr << "Pulsar::ReceptionCalibrator::calibrate unimplemented" << endl;
}

