#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Archive.h"

#include "Calibration/Gain.h"
#include "Calibration/Polar.h"

#include "Calibration/ReceptionModelAxisPlotter.h"

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
  plot_constraints (ichan, calibrator->calibrator_estimate.input_index,
		    calibrator->model[ichan]->ArtificialCalibrator_path);
}

void Pulsar::ReceptionCalibratorPlotter::plot_psr_constraints (unsigned ichan,
							       unsigned istate)
{
  plot_constraints (ichan, istate, calibrator->model[ichan]->Pulsar_path);
}

void Pulsar::ReceptionCalibratorPlotter::plot_constraints (unsigned ichan,
							   unsigned istate,
							   unsigned ipath)
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

  Calibration::ReceptionModelAxisPlotter<MJD> plotter;

  plotter.set_model( calibrator->model[ichan]->equation );
  plotter.set_model_solved( calibrator->get_solved() );
  plotter.set_parallactic( &(calibrator->model[ichan]->parallactic) );

  plotter.set_isource (istate);
  plotter.set_ipath (ipath);

  plotter.set_axis( &(calibrator->model[ichan]->time) );
  plotter.set_min ( calibrator->start_epoch );
  plotter.set_max ( calibrator->end_epoch );
  plotter.set_npt ( 100 );

  if (!calibrator->get_solved()) {

    char buffer[256];
    sprintf (buffer, "pcm_state%d_chan%d.dat", istate, ichan);

    plotter.set_output (buffer);

  }

  plotter.plot_observations ();

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

  if (verbose)
    cerr << "Pulsar::ReceptionCalibratorPlotter::plotcal call plot" << endl;

  plot( new ReceptionCalibrator::CalInfo(calibrator),
	calibrator->get_nchan(),
	calibrator->get_Archive()->get_centre_frequency(),
	calibrator->get_Archive()->get_bandwidth() );

}

Pulsar::ReceptionCalibrator::CalInfo::CalInfo (ReceptionCalibrator* cal)
{
  calibrator = cal;
}

//! Return the number of channels in the calibrator estimate
unsigned Pulsar::ReceptionCalibrator::CalInfo::get_nchan () const
{
  return calibrator->calibrator_estimate.source.size();
}

//! Return the number of channels in the flux calibrator estimate
unsigned Pulsar::ReceptionCalibrator::CalInfo::get_fcal_nchan () const
{
  return calibrator->flux_calibrator_estimate.source.size();
}

//! Return the number of parameter classes
unsigned Pulsar::ReceptionCalibrator::CalInfo::get_nclass () const
{
  unsigned classes = 0;

  if (get_nchan())
    classes = 1;

  if (get_fcal_nchan()) {
    if (calibrator->measure_cal_V)
      classes += 1; 
    else
      classes +=2;
  }

  return classes;
}

//! Return the name of the specified class
const char*
Pulsar::ReceptionCalibrator::CalInfo::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "Noise Diode";
  case 1:
    return "System + Hydra A";
  case 2:
    return "Hydra A (Q,U,V)";
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
  if (iclass == 0) {
    if (ichan >= get_nchan())
      throw Error (InvalidParam, 
                   "Pulsar::ReceptionCalibrator::CalInfo::get_param",
                   "ichan=%d >= nchan=%d", ichan, get_nchan());

    return calibrator->calibrator_estimate.source[ichan].get_Estimate(iparam);
  }

  else {
    if (ichan >= get_fcal_nchan())
      throw Error (InvalidParam,
                   "Pulsar::ReceptionCalibrator::CalInfo::get_param",
                   "ichan=%d >= fcal_nchan=%d", ichan, get_fcal_nchan());

    if (iclass == 1)
      return calibrator->flux_calibrator_estimate.source[ichan].get_Estimate(iparam);
    else if (iclass == 2)
      return calibrator->flux_calibrator_estimate.source[ichan].get_Estimate(iparam+1);
  }

  return 0.0;
}

