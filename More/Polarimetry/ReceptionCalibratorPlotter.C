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

  unsigned ipt = 0, npt = calibrator->get_nchan();

  if (npt == 0) {
    cerr << "Pulsar::ReceptionCalibratorPlotter::plot no points to plot"
	 << endl;
    return;
  }

  float xmin, xmax, ymin, ymax;
  cpgqvp (0, &xmin, &xmax, &ymin, &ymax);

  float ybottom = ymin;
  float yrange = ymax - ymin;
  float yspace = 0.1 * yrange;
  float yheight = (yrange - yspace) / 3.0;

  cpgsci(1);
  cpgslw(1);
  cpgsch(1);

  // the plotting class
  EstimatePlotter plotter;

  // the data to be plotted
  vector< Estimate<float> > data (npt);

  unsigned ndim = 4;
  unsigned idim = 0;

  for (idim=0; idim<ndim; idim++) {
    for (ipt=0; ipt<npt; ipt++)
      data[ipt] = calibrator->calibrator_estimate.source[ipt].get_Estimate(idim);

    plotter.add_plot (data);
  }

  cpgsvp(xmin, xmax, ybottom, ybottom + yheight);
  for (idim=0; idim<ndim; idim++) {
    cpgsci (idim+1);
    plotter.plot (idim);
  }

  cpgsci (1);
  cpgbox("bcst",0,0,"bcnvst",0,0);
  cpgmtxt("L",2.5,.5,.5,"Stokes");

  // restore the viewport
  cpgsvp (xmin, xmax, ymin, ymax);

}

