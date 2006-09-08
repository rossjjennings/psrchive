/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluxPlot.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"

#include <cpgplot.h>

Pulsar::FluxPlot::FluxPlot ()
{
  isubint = ichan = ipol = 0;
  plot_ebox = false;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
}
 
TextInterface::Class* Pulsar::FluxPlot::get_interface ()
{
  return new Interface (this);
}

/*! The ProfileVectorPlotter class computes the minimum and maximum values
  to be plotted */
void Pulsar::FluxPlot::prepare (const Archive* data)
{
  plotter.profiles.clear();

  // derived classes fill the plotter.profiles attribute
  get_profiles (data);

  plotter.minmax (get_frame());
}


/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::FluxPlot::draw (const Archive* data)
{
  // PhaseScale::get_ordinates fills the x-axis with turns/deg/milliseconds ...
  get_scale()->get_ordinates (data, plotter.x);

  plotter.draw ();

  cpgsci (1);
  if (plot_ebox)
    plot_error_box (data);
}


//! Scale in on the on-pulse region
void Pulsar::FluxPlot::auto_scale_phase (const Profile* profile, float buf)
{
  int rise, fall;
  profile->find_peak_edges (rise, fall);

  cerr << "AUTO ZOOM rise=" << rise << " fall=" << fall << endl;

  float nbin = profile->get_nbin ();

  float start = rise / nbin;
  float stop = fall / nbin;

  cerr << "AUTO ZOOM phase rise=" << start << " fall=" << stop << endl;

  if (start > stop)
    stop += 1.0;

  buf = (stop - start) * buf;
  
  stop += buf;
  start -= buf;

  cerr << "AUTO ZOOM fixed rise=" << start << " fall=" << stop << endl;

  get_frame()->get_x_scale()->set_range_norm (start, stop);
}

template<typename T> T sqr (T x) { return x*x; }

//! Return the label for the y-axis
std::string Pulsar::FluxPlot::get_ylabel (const Archive* data)
{
  if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else
    return "Relative Flux Units";
}

float Pulsar::FluxPlot::get_phase_error (const Archive* data)
{
  double dm        = data->get_dispersion_measure();
  double freq      = data->get_centre_frequency();
  double chan_bw   = data->get_bandwidth() / data->get_nchan();
  double period    = data->get_Integration(0)->get_folding_period();

  cerr << "Frequency = " << freq << endl;
  cerr << "Channel bandwidth = " << chan_bw << endl;
  cerr << "DM = " << dm << endl;
  cerr << "period = " << period*1e3 << " ms" << endl;

  // DM smearing in seconds
  double dm_smearing = dispersion_smear (dm, freq, chan_bw);
  cerr << "Dispersion smearing = " << dm_smearing*1e3 << " ms" << endl;

  // Scattering in seconds
  double scattering = pow(dm/1000,3.5) * pow(400/freq,4);
  cerr << "Predicted scattering = " << scattering*1e3 << " ms" << endl;

  // Time resolution in seconds
  double time_res = period / data->get_nbin();
  cerr << "Time resolution = " << time_res*1e3 << " ms" << endl;

  time_res = sqrt( sqr(dm_smearing) + sqr(scattering) + sqr(time_res) );
  cerr << "TOTAL resolution = " << time_res*1e3 << " ms" << endl;
  
  float x_error = time_res / period;
  cerr << "Phase error = " << x_error << endl;

  return x_error;
}

float Pulsar::FluxPlot::get_flux_error (const Profile* profile)
{
  float min_phase = profile->find_min_phase();
  double var;
  profile->stats (min_phase, 0, &var);

  // sigma error box
  float y_error = sqrt(var);
  cerr << "Flux error = " << y_error << endl;

  return y_error;
}


void Pulsar::FluxPlot::plot_error_box (const Archive* data)
{
  float y_error = get_flux_error (plotter.profiles[0]);
  float x_error = get_phase_error (data);

  float x_min = 0;
  float x_max = 0;

  get_scale()->get_range (data, x_min, x_max);

  float xscale = get_scale()->get_scale (data);

  float x1 = x_min + (x_max - x_min) * 0.05;
  float x2 = x1 + x_error * xscale;
  float y1 = 10 * y_error;
  float y2 = y1 + 4.0 * y_error;

  cpgmove (x1,y1);
  cpgdraw (x2,y1);
  cpgdraw (x2,y2);
  cpgdraw (x1,y2);
  cpgdraw (x1,y1);
}
