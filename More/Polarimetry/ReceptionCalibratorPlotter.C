#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Archive.h"

#include "Calibration/Gain.h"
#include "Calibration/Polar.h"

#include "EstimatePlotter.h"

#include <cpgplot.h>

Pulsar::ReceptionCalibratorPlotter::ReceptionCalibratorPlotter
(ReceptionCalibrator* cal)
{
  calibrator = cal;
}

Pulsar::ReceptionCalibratorPlotter::~ReceptionCalibratorPlotter ()
{
}

void Pulsar::ReceptionCalibratorPlotter::plot_cal_constraints (unsigned ichan)
{
  plot_constraints (ichan, calibrator->calibrator_estimate.source_index);
}

void Pulsar::ReceptionCalibratorPlotter::plot_constraints (unsigned ichan,
							   unsigned istate)
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
                 "ReceptionCalibrator not set");

  if (istate >= calibrator->get_nstate())
    throw Error (InvalidRange,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
		 "istate=%d >= nstate=%d", istate, calibrator->get_nstate());

  if (ichan >= calibrator->get_nchan())
    throw Error (InvalidRange,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
		 "ichan=%d >= nchan=%d", ichan, calibrator->get_nchan());

  // extract the appropriate equation
  const Calibration::ReceptionModel* equation;
  equation = calibrator->model[ichan]->equation;

  const Calibration::Parallactic* parallactic;
  parallactic = &(calibrator->model[ichan]->parallactic);

  vector< Estimate<float> > stokes[4];
  vector< float > para;

  unsigned nmeas = equation->get_ndata ();
  for (unsigned imeas=0; imeas<nmeas; imeas++) {

    //! Get the specified Measurements
    const Calibration::Measurements& data
      = equation->get_data (imeas);

    data.set_coordinates();

    unsigned mstate = data.size();

    for (unsigned jstate=0; jstate<mstate; jstate++)
      if (data[jstate].source_index == istate) {
	for (unsigned ipol=0; ipol<4; ipol++) {
	  stokes[ipol].push_back (Estimate<float>());
	  stokes[ipol].back() = data[jstate][ipol];
	}

	para.push_back ( parallactic->get_param(0) * 180.0/M_PI );

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

  string label;
  char buffer [256];

  if (istate == 0)
    label = "Calibrator Constraints";
  else {
    label = "Pulsar Constraints";
    unsigned phase_bin = calibrator->pulsar[istate-1].phase_bin;
    sprintf (buffer, " phase bin %d", phase_bin);
    label += buffer;
  }

  sprintf (buffer, " freq. channel %d", ichan);
  label += buffer;

  cpglab ("Parallactic Angle (degrees)", "Uncalibrated Stokes", label.c_str());

  if (!calibrator->get_solved())
    return;

  plot_model (ichan, istate);
}



void Pulsar::ReceptionCalibratorPlotter::plot_model (unsigned ichan,
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
  Calibration::ReceptionModel* equation = calibrator->model[ichan]->equation;
  Calibration::Parallactic* para = &(calibrator->model[ichan]->parallactic);

  equation->set_source (istate);

  bool plot_model_points = false;

  unsigned nmeas = equation->get_ndata ();
  for (unsigned imeas=0; imeas<nmeas; imeas++) {

    //! Get the specified Measurements
    const Calibration::Measurements& data = equation->get_data (imeas);

    unsigned mstate = data.size();
    bool was_measured = false;

    for (unsigned jstate=0; jstate<mstate; jstate++)
      if (data[jstate].source_index == istate)
        was_measured = true;

    if (!was_measured)
      continue;

    // set the signal path through which these measurements were observed
    equation->set_path (data.path_index);

    if (!plot_model_points)
      break;

    // set the independent variables for this set of measurements
    data.set_coordinates();

    float parallactic = para->get_param(0) * 180.0/M_PI;

    Stokes<float> stokes = equation->evaluate ();

    for (unsigned ipol=0; ipol<4; ipol++) {
      cpgsci (ipol+1);
      cpgpt1 (parallactic, stokes[ipol], 5);
    }

  }
  

  unsigned npt = 100;

  vector<float> parallactic (npt);
  vector<Stokes<float> > stokes (npt);

  MJD start = calibrator->start_epoch;
  MJD end = calibrator->end_epoch;
  MJD step = (end-start)/npt;

  for (unsigned ipt=0; ipt<npt; ipt++) {

    MJD epoch = start + step * ipt;

    calibrator->model[ichan]->time.send (epoch);

    parallactic[ipt] = para->get_param(0);

    stokes[ipt] = equation->evaluate ();
    parallactic[ipt] *= 180.0/M_PI;

  }

  for (unsigned ipol=0; ipol<4; ipol++) {
    cpgsci (ipol+1);
    
    cpgmove (parallactic[0], stokes[0][ipol]);
    for (unsigned ipt=1; ipt<npt; ipt++)
      cpgdraw (parallactic[ipt], stokes[ipt][ipol]);
    
  }

}

void Pulsar::ReceptionCalibratorPlotter::plot_phase_constraints ()
{
  unsigned nbin = calibrator->calibrator->get_nbin();

  unsigned nstate = calibrator->pulsar.size();

  cpgswin (0,1,0,1);

  for (unsigned istate=0; istate<nstate; istate++) {

    float phase = calibrator->pulsar[istate].phase_bin;
    phase /= nbin;

    cpgmove (phase, 0.1);
    cpgdraw (phase, 0.9);

  }
}


//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::ReceptionCalibratorPlotter::plotcal ()
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::ReceptionCalibratorPlotter::plotcal",
                 "ReceptionCalibrator not set");

  plot( new ReceptionCalibrator::CalInfo(calibrator),
	calibrator->get_nchan(),
	calibrator->get_Archive()->get_centre_frequency(),
	calibrator->get_Archive()->get_bandwidth() );

}

Pulsar::ReceptionCalibrator::CalInfo::CalInfo (ReceptionCalibrator* cal)
{
  calibrator = cal; 
}
      
//! Return the number of parameter classes
unsigned Pulsar::ReceptionCalibrator::CalInfo::get_nclass () const
{
  if (calibrator->flux_calibrator_estimate.source.size() != 0) {
    if (calibrator->measure_cal_V)
      return 2; 
    else
      return 3;
  }
  else
    return 1;
}

//! Return the name of the specified class
const char*
Pulsar::ReceptionCalibrator::CalInfo::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "Calibrator";
  case 1:
    return "Flux Calibrator";
  case 2:
    return "Flux Stokes";
  default:
    return "";
  }
}

//! Return the number of parameters in the specified class
unsigned
Pulsar::ReceptionCalibrator::CalInfo::get_nparam (unsigned iclass) const
{
  if (calibrator->measure_cal_V && iclass < 2)
    return 4;

  else if (!calibrator->measure_cal_V) {

    switch (iclass) {
    case 0:
      return 4;
    case 1:
      return 1;
    case 2:
      return 3;
    default:
      return 0;
    }

  }
    
  return 0;
}

//! Return the estimate of the specified parameter
Estimate<float>
Pulsar::ReceptionCalibrator::CalInfo::get_param (unsigned ichan,
						 unsigned iclass,
						 unsigned iparam) const
{
  switch (iclass) {
  case 0:
    return calibrator->calibrator_estimate.source[ichan].get_Estimate(iparam);
  case 1:
    return calibrator->flux_calibrator_estimate.source[ichan].get_Estimate(iparam);
  case 2:
    return calibrator->flux_calibrator_estimate.source[ichan].get_Estimate(iparam+1);
  default:
    return 0.0;
  }
}
