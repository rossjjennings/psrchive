#include "ReceptionCalibratorPlotter.h"
#include "ReceptionCalibrator.h"
#include "EstimatePlotter.h"

#include <cpgplot.h>

Pulsar::ReceptionCalibratorPlotter::~ReceptionCalibratorPlotter ()
{
}

//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::ReceptionCalibratorPlotter::init (const Calibrator* calib)
{
  const ReceptionCalibrator* temp;
  temp = dynamic_cast<const ReceptionCalibrator*> (calib);
  if (!temp) {
    cerr << "Pulsar::ReceptionCalibratorPlotter::init"
      " Calibrator is not a ReceptionCalibrator" << endl;
    return;
  }

  calibrator = const_cast<ReceptionCalibrator*> (temp);

  calibrator->initialize();

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

void Pulsar::ReceptionCalibratorPlotter::plot_constraints ()
{
  plot_constraints (calibrator->get_nchan()/2, calibrator->get_nstate()/2);
}

void Pulsar::ReceptionCalibratorPlotter::plot_cal_constraints ()
{
  plot_cal_constraints (calibrator->get_nchan()/2);
}

void Pulsar::ReceptionCalibratorPlotter::plot_cal_constraints (unsigned ichan)
{
  plot_constraints (ichan, calibrator->calibrator_state_index);
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

  // extract the appropriate equation
  const Calibration::SAtPEquation* equation = calibrator->equation[ichan];

  unsigned nmeas = equation->get_nmeasurements ();

  vector< Estimate<float> > stokes[4];
  vector< float > para;

  for (unsigned imeas=0; imeas<nmeas; imeas++) {

    //! Get the specified Measurements
    const Calibration::Measurements& measurements
      = equation->get_measurements (imeas);

    double xval = measurements.interval[0];

    unsigned nstate = measurements.size();

    for (unsigned jstate=0; jstate<nstate; jstate++)
      if (measurements[jstate].state_index == istate) {

	for (unsigned ipol=0; ipol<4; ipol++) {
	  stokes[ipol].push_back (Estimate<float>());
	  stokes[ipol].back().val = measurements[jstate].val[ipol];
	  stokes[ipol].back().var = measurements[jstate].var;
	}

	calibrator->parallactic.set_abscissa (0, xval);
	para.push_back ( calibrator->parallactic.get_param(0) * 180.0/M_PI );

	break;
      }
  }

  // the plotting class
  EstimatePlotter plotter;
  unsigned ipol;

  for (ipol=0; ipol<4; ipol++)
    plotter.add_plot (para, stokes[ipol]);

  for (ipol=0; ipol<4; ipol++) {
    cpgsci (ipol+1);
    plotter.plot (ipol);
  }

  cpgsci (1);
  cpgbox ("bcnst",0,0,"bcnst",0,0);

  cpglab ("Parallactic Angle (degrees)",
	  "Uncalibrated Stokes",
	  "Self-Calibration Constraints");

}
