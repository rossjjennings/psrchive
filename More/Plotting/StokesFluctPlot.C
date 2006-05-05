/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesFluctPlot.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/Fourier.h"

Pulsar::StokesFluctPlot::StokesFluctPlot ()
{
  plot_values  = "Ip";
  plot_colours = "123";
  plot_lines   = "111";
}

TextInterface::Class* Pulsar::StokesFluctPlot::get_interface ()
{
  return new Interface (this);
}

Pulsar::Profile* new_Fluct (const Pulsar::PolnProfile* data, char code)
{
  Reference::To<Pulsar::Profile> profile;

  switch (code) {
  case 'I':
    return data->get_Profile(0)->clone();
  case 'Q':
    return data->get_Profile(1)->clone();
  case 'U':
    return data->get_Profile(2)->clone();
  case 'V':
    return data->get_Profile(3)->clone();

  case 'L': {
    // total linearly polarized flux
    profile = data->get_Profile(1)->clone();
    profile->sum (data->get_Profile(2));
    return profile.release();
  }

  case 'p': {
    // total polarized flux
    profile = data->get_Profile(1)->clone();
    profile->sum (data->get_Profile(2));
    profile->sum (data->get_Profile(3));
    return profile.release();
  }

  case 'S': {
    // polarimetric invariant flux
    profile = new Pulsar::Profile;
    data->invint(profile);
    return profile.release();
  }

  default:
    throw Error (InvalidParam, "Pulsar::get_Profile",
		 "Unknown polarization code '%c'", code);
  }

}

void Pulsar::StokesFluctPlot::get_profiles (const Archive* data)
{
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
    cerr << "Pulsar::StokesFluctPlot::get_profiles calling get_Stokes" << endl;

  Reference::To<const PolnProfile> profile = get_Stokes (data, isubint, ichan);

  if (verbose)
    cerr << "Pulsar::StokesFluctPlot::get_profiles filling vector" << endl;

  for (unsigned ipol=0; ipol < plotter.profiles.size(); ipol++) {

    Reference::To<Profile> prof;

    // special case for invariant interval: form before FFT
    if (plot_values[ipol] == 'S') {
      prof = new Profile;
      profile->invint (prof);
      prof = fourier_transform (prof);
      detect (prof);
    }

    // all other cases: FFT before formation
    else {
      Reference::To<PolnProfile> fft = fourier_transform (profile);
      detect (fft);
      prof = new_Fluct (fft, plot_values[ipol]);
    }

    prof->logarithm();
    plotter.profiles[ipol] = prof;
    plotter.plot_sci[ipol] = plot_colours[ipol] - '0';
    plotter.plot_sls[ipol] = plot_lines[ipol] - '0';
  }
}

