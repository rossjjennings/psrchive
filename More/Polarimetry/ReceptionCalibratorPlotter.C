#include "ReceptionCalibratorPlotter.h"
#include "ReceptionCalibrator.h"

Pulsar::ReceptionCalibratorPlotter::~ReceptionCalibratorPlotter ()
{
}

//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::ReceptionCalibratorPlotter::init (const Calibrator* calib)
{
  calibrator = dynamic_cast<const ReceptionCalibrator*> (calib);
  if (!calibrator) {
    cerr << "Pulsar::ReceptionCalibratorPlotter::init"
      " Calibrator is not a ReceptionCalibrator" << endl;
    return;
  }
}

//! Get the number of data points to plot
unsigned Pulsar::ReceptionCalibratorPlotter::get_ndat () const
{
  return calibrator->equation.size();
}

//! Get the number of boost parameters
unsigned Pulsar::ReceptionCalibratorPlotter::get_nboost () const
{
  return 3;
}

//! Get the number of rotation parameters
unsigned Pulsar::ReceptionCalibratorPlotter::get_nrotation () const
{
  return 3;
}

//! Get the gain for the specified point
Estimate<float>
Pulsar::ReceptionCalibratorPlotter::get_gain (unsigned idat)
{
  return calibrator->equation[idat]->get_receiver()->get_gain();
}

//! Get the gain for the specified point
Estimate<float>
Pulsar::ReceptionCalibratorPlotter::get_boost (unsigned idat, unsigned iboost)
{
  return calibrator->equation[idat]->get_receiver()->get_boostGibbs(iboost);
}

    //! Get the gain for the specified point
Estimate<float>
Pulsar::ReceptionCalibratorPlotter::get_rotation (unsigned idat, unsigned irot)
{
  return calibrator->equation[idat]->get_receiver()->get_rotationEuler(irot);
}
