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
  Calibration::ReceptionModel* equation;
  equation = calibrator->model[ichan]->equation;

  Calibration::Parallactic* parallactic;
  parallactic = &(calibrator->model[ichan]->parallactic);

  vector< Estimate<float> > stokes[4];
  vector< float > para;

  unsigned nmeas = equation->get_ndata ();
  bool found = false;

  for (unsigned imeas=0; imeas<nmeas; imeas++) {

    // Get the specified Measurements
    const Calibration::Measurements& data = equation->get_data (imeas);

    // Set the parallactic angle
    data.set_coordinates();

    unsigned mstate = data.size();

    for (unsigned jstate=0; jstate<mstate; jstate++)
      if (data[jstate].source_index == istate) {

	for (unsigned ipol=0; ipol<4; ipol++)
	  stokes[ipol].push_back (Estimate<float>(data[jstate][ipol]));
    
	para.push_back ( parallactic->get_param(0) * 180.0/M_PI );

	if (!found) {
	  // set the data path and source
	  equation->set_path (data.path_index);
	  equation->set_source (istate);
	  found = true;
	}

      }

  }

  if (stokes[0].size() == 0) {
    cerr << "Pulsar::ReceptionCalibratorPlotter::plot_constraints "
            "ichan=" << ichan << " istate=" << istate << " no data" << endl;
    return;
  }


  if (!calibrator->get_solved()) {

    char buffer[256];
    sprintf (buffer, "pcm_state%d_chan%d.dat", istate, ichan);

    FILE* fptr = fopen (buffer, "w");
    if (!fptr)
      throw Error (FailedSys, "plot", "fopen (%s)", buffer);

    for (unsigned ipt=0; ipt < para.size(); ipt++) {
      fprintf (fptr, "%f ", para[ipt]);

      for (unsigned ipol=0; ipol<4; ipol++)
	fprintf (fptr, "%f %f ", stokes[ipol][ipt].val, stokes[ipol][ipt].var);

      fprintf (fptr, "\n");

    }

    fclose (fptr);

  }


  // the plotting class
  EstimatePlotter plotter;

  if (!use_colour) {

    float x1, x2, y1, y2;

    // query the current size of the viewport in normalized device coordinates
    cpgqvp (0, &x1, &x2, &y1, &y2);
    cpgslw (2);

    float Ispace = 0.18;
    float space = 0.02;
    
    float Iscale = 0.05;
    float Sscale = 0.2;
    
    float xborder  = 0.03;
    float Iyborder = 0.2;
    float Syborder = 0.08;
    
    cpgsvp (x1, x2, y1, y1+(y2-y1)*Ispace);
    
    plotter.add_plot (para, stokes[0]);
    plotter.set_border (xborder, Iyborder);
    plot_stokes (plotter, 0, ichan, 0, Iscale);
    
    plotter.clear ();
    plotter.set_border (xborder, Syborder);
    
    cpgsvp (x1, x2, y1+(y2-y1)*(Ispace+space), y2);
    
    for (unsigned ipol=1; ipol<4; ipol++)
      plotter.add_plot (para, stokes[ipol]);
    
    plotter.separate_viewports();
    
    for (unsigned iplot=0; iplot<3; iplot++)
      plot_stokes (plotter, iplot, ichan, iplot+1, Sscale);
    
    cpgsci (1);
    cpgsls (1);
    cpgsvp (x1, x2, y1, y2);
    cpgbox ("bcnst",0,0,"",0,0);
    cpgmtxt("B",3.0,.5,.5,"Parallactic Angle (degrees)");
    
  }

  else {

    unsigned ipol;
    
    for (ipol=0; ipol<4; ipol++)
      plotter.add_plot (para, stokes[ipol]);
    
    for (ipol=0; ipol<4; ipol++) {
      cpgsci( ipol+1 );
      plotter.plot (ipol);
    }

    cpgsci (1);
    cpgsls (1);

    string label;

    cpgbox ("bcnst",0,0,"bcnst",0,0);

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
    
    cpglab ( "Parallactic Angle (degrees)",
	     "Normalized Stokes Parameters",
	     label.c_str());

  }

  cpgsci (1);
  cpgsls (1);

}

void Pulsar::ReceptionCalibratorPlotter::plot_stokes (EstimatePlotter& plotter,
						      unsigned iplot,
						      unsigned ichan,
						      unsigned ipol,
						      float spacing)
{
  char stokes_label[64] = "\\fiS'\\b\\d\\fnk";
  int  position = strlen (stokes_label) - 1;
  char* stokes_index = "0123";

  plotter.plot (iplot);

  if (calibrator->get_solved())
    plot_model (ichan, ipol);

  cpgbox ("bcst",0,0,"bcvnst",spacing,2);

  cerr << "stokes=" << stokes_index[ipol] << endl;
  cerr << "label=" << stokes_label[0] << endl;
  
  stokes_label[position] = stokes_index[ipol];
  cerr << "poo" << endl;
  cerr << "stokes label=" << stokes_label << endl;
  
  cpgmtxt("L",3.5,.5,.5,stokes_label);

  // possible circumflex, \\(0756,0832,2247)
  cpgsch(1.05);
  cpgmtxt("L",3.5,.5,.5,"\\u\\(2247)\\d \\(2197)");
  cpgsch(1.0);
}

void Pulsar::ReceptionCalibratorPlotter::plot_model (unsigned ichan,
						     unsigned ipol,
						     unsigned npt)
{
  // extract the appropriate equation
  const Calibration::ReceptionModel* equation;
  equation = calibrator->model[ichan]->equation;

  const Calibration::Parallactic* para;
  para = &(calibrator->model[ichan]->parallactic);

  MJD start = calibrator->start_epoch;
  MJD end = calibrator->end_epoch;
  MJD step = (end-start)/npt;

  for (unsigned ipt=0; ipt<npt; ipt++) {

    MJD epoch = start + step * ipt;

    calibrator->model[ichan]->time.send (epoch);

    float parallactic = para->get_param(0) * 180.0/M_PI;
    Stokes<float> stokes = equation->evaluate ();

    if (ipt == 0)
      cpgmove (parallactic, stokes[ipol]);
    else
      cpgdraw (parallactic, stokes[ipol]);
    
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
