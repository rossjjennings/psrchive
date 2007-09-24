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
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/SmoothMean.h"

#include "Physical.h"

#include <cpgplot.h>

using namespace std;

Pulsar::FluxPlot::FluxPlot ()
{
  isubint = ichan = ipol = 0;
  auto_zoom = 0;
  baseline_zoom = 0;
  original_nchan = 0;
  plot_ebox = false;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
}

Pulsar::FluxPlot::~FluxPlot ()
{
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

  if (auto_zoom)
    auto_scale_phase (plotter.profiles[0], auto_zoom);

  if (baseline_zoom)
    selection = plotter.profiles[0]->baseline();

  if (selection) {
    if (verbose)
      cerr << "Pulsar::FluxPlot::prepare using selected bins" << endl;

    frame->get_y_scale()->set_minmax (selection->get_min(),
				      selection->get_max());
  }
  else {
    if (verbose)
      cerr << "Pulsar::FluxPlot::prepare using all bins" << endl;
    plotter.minmax (get_frame());
  }
}



/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::FluxPlot::draw (const Archive* data)
{
  // PhaseScale::get_ordinates fills the x-axis with turns/deg/milliseconds ...
  get_scale()->get_ordinates (data, plotter.ordinates);

  pair<float,float> range = get_frame()->get_x_scale()->get_range_norm();
  plotter.draw ( range.first, range.second );

  cpgsci (1);
  if (plot_ebox)
    plot_error_box (data);

  if (selection)
    plot_selection (data);
  
}

void Pulsar::FluxPlot::plot_selection (const Archive* data)
{
  vector<float> x;
  get_scale()->get_ordinates (data, x);

  const float* amps = plotter.profiles[0]->get_amps();

  pair<float,float> range = get_scale()->get_range_norm();

  int range_start = int(floor(range.first));
  int range_end = int(ceil(range.second));

  cpgsci (5);

  for( int range = range_start; range < range_end; range ++ )
  {
    float xoff = float(range) * get_scale()->get_scale(data);

    for (unsigned i=0; i<x.size(); i++)
      if ((*selection)[i])
	cpgpt1 (x[i] + xoff, amps[i], 17);
  }

  float x_min = 0;
  float x_max = 0;
  get_scale()->PlotScale::get_range (x_min, x_max);

  Estimate<double> mean = selection->get_mean ();
  Estimate<double> var = selection->get_variance ();
  double error = sqrt (mean.get_variance() + var.get_value());

  cpgsci (4);
  cpgmove (x_min, mean.get_value());
  cpgdraw (x_max, mean.get_value());

  cpgsls (2);

  for (double offset=-2*error; offset <= 2*error; offset+=error) {
    cpgmove (x_min, mean.get_value() + offset);
    cpgdraw (x_max, mean.get_value() + offset);
  }



}

void Pulsar::FluxPlot::set_selection (PhaseWeight* w)
{
  selection = w; 
}

/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::FluxPlot::plot_profile (const Profile* data)
{
  plotter.profiles.clear();
  plotter.profiles.push_back (data);
  plotter.minmax (get_frame());

  get_frame()->focus (0);

  get_scale()->get_ordinates (0, plotter.ordinates);

  pair<float,float> range = get_frame()->get_x_scale()->get_range_norm();
  plotter.draw ( data, range.first, range.second );
}

//! Scale in on the on-pulse region
void Pulsar::FluxPlot::auto_scale_phase (const Profile* profile, float buf)
{
  int rise, fall;
  profile->find_peak_edges (rise, fall);

  float nbin = profile->get_nbin ();
  float start = rise / nbin;
  float stop = fall / nbin;

  if (start > stop)
    stop += 1.0;

  cerr << "AUTO ZOOM rise=" << rise << " fall=" << fall 
       << " nbin=" << nbin << endl
       << "AUTO ZOOM phase rise=" << start << " fall=" << stop << endl;

  double mean = (stop + start) * 0.5;
  double diff = (stop - start) * 0.5;

  start = mean - buf * diff;
  stop  = mean + buf * diff;

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

  if (original_nchan)
    chan_bw = data->get_bandwidth() / original_nchan;

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
  cerr << "Phase error = " << x_error << " turns" << endl;

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

  get_scale()->PlotScale::get_range (x_min, x_max);

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

#include "Pulsar/InfoLabel.h"

void Pulsar::FluxPlot::set_info_label (bool flag)
{
  if (flag) {
    info_label = new InfoLabel (this);
    get_frame()->set_label_above( info_label );
    get_frame()->get_label_below()->set_all(PlotLabel::unset);
  }
  else
    get_frame()->set_label_above( new PlotLabel );
}

bool Pulsar::FluxPlot::get_info_label () const
{
  return info_label;
}

