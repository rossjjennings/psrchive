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

void Pulsar::ReceptionCalibratorPlotter::plot_constraints (unsigned ichan,
							   unsigned istate)
{
  if (istate >= calibrator->get_nstate())
    throw Error (InvalidRange,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
		 "istate=%d >= nstate=%d", istate, calibrator->get_nstate());

  if (ichan >= calibrator->get_nchan())
    throw Error (InvalidRange,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
		 "ichan=%d >= nchan=%d", ichan, calibrator->get_nchan());

}

#if 0
  //! Polarimetric measurement, estimated error, and state identification code
  class MeasuredState : public Estimate<Stokes<double>, double> {
  public:
    MeasuredState () { state_index = 0; }
    unsigned state_index;
  };
 
  //! A set of measurements made at the same abscissa interval
  class Measurements : public vector<MeasuredState> {
  public:

    //! Construct measurement with single abscissa dimension
    Measurements (double x0 = 0.0);

    //! The coordinates of the measurement
    vector<double> interval;
  };
#endif

