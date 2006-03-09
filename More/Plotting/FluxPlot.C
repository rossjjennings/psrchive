#include "Pulsar/FluxPlotterTI.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"

#include <cpgplot.h>

Pulsar::FluxPlotter::FluxPlotter ()
{
  isubint = ichan = 0;
  plot_ebox = false;
  plot_histogram = false;

  get_frame()->get_y_axis()->set_buf_norm(0.05);
}
 
TextInterface::Class* Pulsar::FluxPlotter::get_interface ()
{
  if (!interface)
    interface = new FluxPlotterTI (this);
  return interface;
}

//! Derived classes must compute the minimum and maximum values (y-axis)
void Pulsar::FluxPlotter::prepare (const Archive* data)
{
  profiles.clear();

  get_profiles (data);

  if (!profiles.size())
    throw Error (InvalidState, "Pulsar::FluxPlotter::prepare",
		 "Profiles array empty after call to get_profiles");

  float x_min = 0.0;
  float x_max = data->get_nbin();
  get_frame()->get_x_axis()->get_range (x_min, x_max);
  int i_min = (int) x_min;
  int i_max = (int) x_max;

  float min = profiles[0]->min(i_min, i_max);
  float max = profiles[0]->max(i_min, i_max);

  for (unsigned iprof=1; iprof < profiles.size(); iprof++) {
    float pmin = profiles[iprof]->min(i_min, i_max);
    float pmax = profiles[iprof]->max(i_min, i_max);
    if (pmin < min)
      min = pmin;
    if (pmax > max)
      max = pmax;
  }

  set_yrange (min, max);
}


//! Derived classes must draw in the current viewport
void Pulsar::FluxPlotter::draw (const Archive* data)
{
  for (unsigned iprof=0; iprof < profiles.size(); iprof++) {

    if (plot_sci.size() == profiles.size())
      cpgsci (plot_sci[iprof]);
    else
      cpgsci (iprof);

    if (plot_sls.size() == profiles.size())
      cpgsls (plot_sls[iprof]);
    else
      cpgsls (iprof);

    draw (profiles[iprof]);
  }

  cpgsci (1);
  if (plot_ebox)
    plot_error_box (data);
}

//! draw the profile in the current viewport and window
void Pulsar::FluxPlotter::draw (const Profile* profile) const
{
  if (plot_histogram)
    cpgbin (profile->get_nbin(), &phases[0], profile->get_amps(), true);
  else
    cpgline (profile->get_nbin(), &phases[0], profile->get_amps());
}



//! Zoom in on the on-pulse region
void Pulsar::FluxPlotter::auto_zoom_phase (const Profile* profile, float buf)
{
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

  get_frame()->get_x_axis()->set_min_norm (start);
  get_frame()->get_y_axis()->set_min_norm (stop);
}

template<typename T> T sqr (T x) { return x*x; }

//! Return the label for the y-axis
std::string Pulsar::FluxPlotter::get_ylabel (const Archive* data)
{
  if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else
    return "Relative Flux Units";
}

float Pulsar::FluxPlotter::get_phase_error (const Archive* data)
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

float Pulsar::FluxPlotter::get_flux_error (const Profile* profile)
{
  float min_phase = profile->find_min_phase();
  double var;
  profile->stats (min_phase, 0, &var);

  // sigma error box
  float y_error = sqrt(var);
  cerr << "Flux error = " << y_error << endl;

  return y_error;
}


void Pulsar::FluxPlotter::plot_error_box (const Archive* data)
{
  float y_error = get_flux_error (profiles[0]);
  float x_error = get_phase_error (data);

  float x_max = get_frame()->get_x_axis()->get_max_norm();
  float x_min = get_frame()->get_x_axis()->get_min_norm();

  float x1 = x_min + (x_max - x_min) * 0.05;
  float x2 = x1 + x_error;
  float y1 = 10 * y_error;
  float y2 = y1 + 4.0 * y_error;

  cpgmove (x1,y1);
  cpgdraw (x2,y1);
  cpgdraw (x2,y2);
  cpgdraw (x1,y2);
  cpgdraw (x1,y1);
}
