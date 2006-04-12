/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesPlot.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"

Pulsar::StokesPlot::StokesPlot ()
{
  plot_values  = "IQUV";
  plot_colours = "1234";
  plot_lines   = "1111";
}

TextInterface::Class* Pulsar::StokesPlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesPlot::get_profiles (const Archive* data)
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
    cerr << "Pulsar::StokesPlot::get_profiles calling get_Stokes" << endl;

  Reference::To<const PolnProfile> profile = get_Stokes (data, isubint, ichan);

  if (verbose)
    cerr << "Pulsar::StokesPlot::get_profiles filling vector" << endl;

  for (unsigned ipol=0; ipol < plotter.profiles.size(); ipol++) {
    plotter.profiles[ipol] = new_Profile (profile, plot_values[ipol]);
    plotter.plot_sci[ipol] = plot_colours[ipol] - '0';
    plotter.plot_sls[ipol] = plot_lines[ipol] - '0';
  }
}

