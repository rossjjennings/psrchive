#include "Pulsar/ProfilePlotter.h"
#include "Pulsar/ProfilePlotterTI.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"

#include <cpgplot.h>

Pulsar::ProfilePlotter::ProfilePlotter ()
{
  isubint = ipol = ichan = 0;

  min_phase = 0.0;
  max_phase = 1.0;

  min_fraction = 0.0;
  max_fraction = 1.0;

  border = 0.05;

  axes = true;
  x_enumerate = true;
  x_label = true;

  y_enumerate = true;
  y_label = true;

  plot_error = false;
}

Pulsar::ProfilePlotter::~ProfilePlotter ()
{
}

TextInterface::Class* Pulsar::ProfilePlotter::get_text_interface ()
{
  if (!text_interface)
    text_interface = new ProfilePlotterTI (this);
  return text_interface;
}

void Pulsar::ProfilePlotter::set_min_phase (float _min_phase)
{ 
  min_phase = _min_phase - floor (_min_phase);
}

void Pulsar::ProfilePlotter::set_max_phase (float _max_phase)
{ 
  max_phase = _max_phase - floor (_max_phase);
}

//! Set the sub-integration to plot (where applicable).
void Pulsar::ProfilePlotter::set_subint (unsigned _isubint)
{
  isubint = _isubint;
}

//! Set the polarization to plot (where applicable).
void Pulsar::ProfilePlotter::set_pol (unsigned _ipol)
{
  ipol = _ipol;
}

//! Set the frequency channel to plot (where applicable).
void Pulsar::ProfilePlotter::set_chan (unsigned _ichan)
{
  ichan = _ichan;
}

//! Zoom in on the on-pulse region
void Pulsar::ProfilePlotter::auto_zoom_phase (const Archive* data, float buf)
{
  const Profile* profile = data->get_Profile (isubint, ipol, ichan);

  int rise, fall;
  profile->find_peak_edges (rise, fall);

  cerr << "AUTO ZOOM rise=" << rise << " fall=" << fall << endl;

  float nbin = profile->get_nbin ();

  float start = rise / nbin;
  float stop = fall / nbin;

  cerr << "AUTO ZOOM phase rise=" << start << " fall=" << stop << endl;

  buf = (stop - start) * buf;
  
  stop += buf;
  start -= buf;

  if (start < 0)
    start = 0;

  if (stop > 1)
    stop = 1;

  set_min_phase (start);
  set_max_phase (stop);
}

template<typename T> T sqr (T x) { return x*x; }

//! Return the label for the y-axis
std::string Pulsar::ProfilePlotter::get_flux_label (const Archive* data)
{
  if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else
    return "Relative Flux Units";
}

//! Return the label for the x-axis
std::string Pulsar::ProfilePlotter::get_phase_label (const Archive* data)
{
  switch (abscissa) {
  case Milliseconds: return "Time (ms)";
  case Degrees: return "Phase (deg.)";
  case Radians: return "Phase (rad.)";
  default:
    return "Pulse Phase";
  }
}

float Pulsar::ProfilePlotter::get_phase_error (const Archive* data)
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

float Pulsar::ProfilePlotter::get_flux_error (const Archive* data)
{
  const Profile* profile = data->get_Profile (isubint, ipol, ichan);
  float min_phase = profile->find_min_phase();
  double var;
  profile->stats (min_phase, 0, &var);

  // sigma error box
  float y_error = sqrt(var);
  cerr << "Flux error = " << y_error << endl;

  return y_error;
}



/*!
  Plots the profile in the currently open pgplot device, using the current
  viewport.  The profile may be rotated, scaled, and zoomed.
  */
void Pulsar::ProfilePlotter::plot (const Archive* data)
{
  float min = 0;
  float max = 0;

  minmax (data, min, max);
 
  float diff = max - min;
  max = min + max_fraction * diff;
  min = min + min_fraction * diff;

  float space = (max - min) * border;
  cpgswin (min_phase, max_phase, min-space, max+space);

  phases.resize (data->get_nbin());
  for (unsigned ibin = 0; ibin < phases.size(); ibin++)
    phases[ibin] = (float(ibin) + 0.5) / phases.size();

  draw (data);

  cpgsls (1);
  cpgsci (1);

  if (plot_error)
    plot_error_box (data);

  if (!axes)
    return;

  float scale = 1.0;

  if (abscissa == Milliseconds)
    scale = data->get_Integration(isubint)->get_folding_period() * 1e3;

  else if (abscissa == Radians)
    scale = 2.0 * M_PI;

  else if (abscissa == Degrees)
    scale = 180.0;

  min_phase *= scale;
  max_phase *= scale;

  cpgswin (min_phase, max_phase, min-space, max+space);

  char* enumerated = "bcnst";
  char* unenumerated = "bcst";

  char* x_num = (x_enumerate) ? enumerated : unenumerated;
  char* y_num = (y_enumerate) ? enumerated : unenumerated;

  cpgbox (x_num,0.0,0, y_num,0.0,0);

  if (y_label)
    cpgmtxt("L",2.5,.5,.5,get_flux_label(data).c_str());

  if (x_label)
    cpgmtxt("B",2.5,.5,.5,get_phase_label(data).c_str());

}

void Pulsar::ProfilePlotter::plot_error_box (const Archive* data)
{
  float y_error = get_flux_error (data);
  float x_error = get_phase_error (data);

  float x1 = min_phase + (max_phase - min_phase) * border;
  float x2 = x1 + x_error;
  float y1 = 10 * y_error;
  float y2 = y1 + 4.0 * y_error;

  cpgmove (x1,y1);
  cpgdraw (x2,y1);
  cpgdraw (x2,y2);
  cpgdraw (x1,y2);
  cpgdraw (x1,y1);
}
