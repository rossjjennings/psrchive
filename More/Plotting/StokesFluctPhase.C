/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesFluctPhase.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/Fourier.h"
#include "Pulsar/Archive.h"

#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/LastSignificant.h"

#include <cpgplot.h>

using namespace std;

Pulsar::StokesFluctPhase::StokesFluctPhase ()
{
  plot_last_harmonic = false;

  plot_values  = "Ip";
  plot_colours = "123";
  plot_lines   = "111";
}

TextInterface::Parser* Pulsar::StokesFluctPhase::get_interface ()
{
  return new Interface (this);
}

/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::StokesFluctPhase::draw (const Archive* data)
{
  FluctPlot::draw (data);
  if (plot_last_harmonic)
  {
    float min, max;
    get_frame()->get_y_scale()->get_minmax (min, max);
    cpgmove (last_harmonic, min);
    cpgdraw (last_harmonic, max*.5);
  }
}


//! Return the label for the y-axis
std::string Pulsar::StokesFluctPhase::get_ylabel (const Archive* data)
{
  return "Fluctuation Phase";
}

namespace Pulsar
{
  void moddet (Profile* moddet, const PolnProfile* data);
}


extern Pulsar::Profile* new_Fluct (const Pulsar::PolnProfile* data, char code);

void Pulsar::StokesFluctPhase::get_profiles (const Archive* data)
{
  /*
    Reference::To<Archive> clone = data->clone();
    clone->centre_max_bin(0);
    data = clone;
  */

  plotter.profiles.resize( plot_values.size() );
  plotter.plot_sci.resize( plot_values.size() );
  plotter.plot_sls.resize( plot_values.size() );
 
  if (plot_values.size() > plot_colours.size())
    throw Error (InvalidState, "Pulsar::StokesPlot::get_profiles",
                 "Mismatch: %u plots and %u colours",
                 plot_values.size(), plot_colours.size());
 
  if (plot_values.size() > plot_lines.size())
    throw Error (InvalidState, "Pulsar::StokesPlot::get_profiles",
                 "Mismatch: %u plots and %u lines",
                 plot_values.size(), plot_lines.size());

  if (verbose)
    cerr << "Pulsar::StokesFluctPhase::get_profiles calling get_Stokes" << endl;

  Reference::To<const PolnProfile> profile;

  if (plot_values != "I")
    profile = get_Stokes (data, isubint, ichan);

  if (verbose)
    cerr << "Pulsar::StokesFluctPhase::get_profiles filling vector" << endl;

  Pulsar::LastSignificant last_significant;

  for (unsigned ipol=0; ipol < plotter.profiles.size(); ipol++)
  {
    Reference::To<Profile> prof;

    if (plot_values == "I")
    {
      Index ipol;
      ipol.set_integrate (true);
      
      Reference::To<const Profile> I;
      I = get_Profile (data, isubint, ipol, ichan);

      prof = fourier_transform (I);
      phase (prof);
    }

    // special case for invariant interval: form before FFT
    else if (plot_values[ipol] == 'S')
    {
      prof = new_Fluct (profile, plot_values[ipol]);
      prof = fourier_transform (prof);
      phase (prof);
    }

    // special case for fluctuation determinant: no detection
    else if (plot_values[ipol] == 'd')
    {
      Reference::To<PolnProfile> fft = fourier_transform (profile);
      prof = new_Fluct (fft, plot_values[ipol]);
    }

    // all other cases: FFT before formation
    else
    {
      Reference::To<PolnProfile> fft = fourier_transform (profile);
      phase (fft);
      prof = new_Fluct (fft, plot_values[ipol]);
    }

    ExponentialBaseline baseline;
    Reference::To<PhaseWeight> weight = baseline.baseline( prof );

    // the standard deviation of an exponential distribution equals its mean
    double rms = weight->get_mean().get_value();

    if (plot_last_harmonic)
    {
      last_significant.find (prof, rms);
      cerr << ipol << ": last harmonic = " << last_significant.get() << endl;
    }
 
    plotter.profiles[ipol] = prof;
    plotter.plot_sci[ipol] = plot_colours[ipol] - '0';
    plotter.plot_sls[ipol] = plot_lines[ipol] - '0';
  }

  last_harmonic = last_significant.get();

  if (verbose)
    cerr << "Pulsar::StokesFluctPhase::get_profiles last harmonic ibin="
	 << last_harmonic << endl;

}

Pulsar::StokesFluctPhase::Interface::Interface (StokesFluctPhase* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluctPlot::Interface() );

  remove ("pol");

  add( &StokesFluctPhase::get_plot_last_harmonic,
       &StokesFluctPhase::set_plot_last_harmonic,
       "last", "Plot the last harmonic marker" );

  add( &StokesFluctPhase::get_plot_values,
       &StokesFluctPhase::set_plot_values,
       "val", "Values to be plotted (IQUVLpS)" );

  add( &StokesFluctPhase::get_plot_colours,
       &StokesFluctPhase::set_plot_colours,
       "sci", "PGPLOT colour index for each value" );
 
  add( &StokesFluctPhase::get_plot_lines,
       &StokesFluctPhase::set_plot_lines,
       "sls", "PGPLOT line style for each value" );

}
