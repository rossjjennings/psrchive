#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Archive.h"

#include "Calibration/Gain.h"
#include "Calibration/Polar.h"

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
  return calibrator->polar[idat]->get_gain();
}

//! Get the gain for the specified point
Estimate<float>
Pulsar::ReceptionCalibratorPlotter::get_boost (unsigned idat, unsigned iboost)
{
  return calibrator->polar[idat]->get_boostGibbs(iboost);
}

    //! Get the gain for the specified point
Estimate<float>
Pulsar::ReceptionCalibratorPlotter::get_rotation (unsigned idat, unsigned irot)
{
  return calibrator->polar[idat]->get_rotationEuler(irot);
}

void Pulsar::ReceptionCalibratorPlotter::plot_constraints (unsigned ichan)
{
  plot_constraints (ichan, calibrator->get_nstate()/4);
}

void Pulsar::ReceptionCalibratorPlotter::plot_cal_constraints (unsigned ichan)
{
  plot_constraints (ichan, calibrator->calibrator.source_index);
}

void Pulsar::ReceptionCalibratorPlotter::plot_constraints (unsigned ichan,
							   unsigned istate)
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
                 "ReceptionCalibrator not set");

  if (istate >= calibrator->get_nstate_pulsar()+1)
    throw Error (InvalidRange,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
		 "istate=%d >= nstate=%d", istate, calibrator->get_nstate());

  if (ichan >= calibrator->get_nchan())
    throw Error (InvalidRange,
		 "Pulsar::ReceptionCalibratorPlotter::plot_constraints",
		 "ichan=%d >= nchan=%d", ichan, calibrator->get_nchan());

  // extract the appropriate equation
  const Calibration::ReceptionModel* equation = calibrator->equation[ichan];

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
	  stokes[ipol].back().val = data[jstate].val[ipol];
	  stokes[ipol].back().var = data[jstate].var;
	}

	para.push_back ( calibrator->parallactic.get_param(0) * 180.0/M_PI );

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
  Calibration::ReceptionModel* equation = calibrator->equation[ichan];

  equation->set_source (istate);

  unsigned nmeas = equation->get_ndata ();
  for (unsigned imeas=0; imeas<nmeas; imeas++) {

    //! Get the specified Measurements
    const Calibration::Measurements& data
      = equation->get_data (imeas);

    unsigned mstate = data.size();
    bool was_measured = false;

    for (unsigned jstate=0; jstate<mstate; jstate++)
      if (data[jstate].source_index == istate)
        was_measured = true;

    if (!was_measured)
      continue;

    equation->initialize (data);

    float para = calibrator->parallactic.get_param(0) * 180.0/M_PI;

    Stokes<float> stokes = equation->evaluate ();

    for (unsigned ipol=0; ipol<4; ipol++) {
      cpgsci (ipol+1);
      cpgpt1 (para, stokes[ipol], 5);
    }

  }
  

  unsigned npt = 100;

  vector<float> para (npt);
  vector<Stokes<float> > stokes (npt);

  MJD start = calibrator->start_epoch;
  MJD end = calibrator->end_epoch;
  MJD step = (end-start)/npt;

  for (unsigned ipt=0; ipt<npt; ipt++) {

    MJD epoch = start + step * ipt;

    calibrator->time.send (epoch);

    para[ipt] = calibrator->parallactic.get_param(0);

    stokes[ipt] = equation->evaluate ();
    para[ipt] *= 180.0/M_PI;

  }

  for (unsigned ipol=0; ipol<4; ipol++) {
    cpgsci (ipol+1);
    
    cpgmove (para[0], stokes[0][ipol]);
    for (unsigned ipt=1; ipt<npt; ipt++)
      cpgdraw (para[ipt], stokes[ipt][ipol]);
    
  }

}

void Pulsar::ReceptionCalibratorPlotter::plot_phase_constraints ()
{
  unsigned nbin = calibrator->uncalibrated->get_nbin();

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
  unsigned ipt = 0, npt = get_ndat ();

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

  // ////////////////////////////////////////////////////////////////////

  for (ipt=0; ipt<npt; ipt++)
    data[ipt] = calibrator->backend[ipt]->get_gamma();

  cpgsvp (xmin, xmax, ybottom, ybottom + yheight);

  plotter.plot (data);

  cpgbox("bcst",0,0,"bcnvst",0,0);
  cpgmtxt("L",2.5,.5,.5,"Boost");

  ybottom += 0.5*yspace + yheight;


  unsigned idim = 0, ndim = 0;

  // ////////////////////////////////////////////////////////////////////

  plotter.clear ();

  for (ipt=0; ipt<npt; ipt++)
    data[ipt] = calibrator->backend[ipt]->get_phi();

  cpgsvp(xmin, xmax, ybottom, ybottom + yheight);

  plotter.plot (data);

  cpgbox("bcst",0,0,"bcnvst",0,0);
  cpgmtxt("L",2.5,.5,.5,"Rotation");

  ybottom += 0.5*yspace + yheight;

  // ////////////////////////////////////////////////////////////////////

  plotter.clear ();

  ndim = 4;
  for (idim=0; idim<ndim; idim++) {
    for (ipt=0; ipt<npt; ipt++)
      data[ipt] = calibrator->calibrator.source[ipt].get_Estimate(idim);

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

