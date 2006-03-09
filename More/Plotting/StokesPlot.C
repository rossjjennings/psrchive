#include "Pulsar/StokesPlotterTI.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"

#include "Physical.h"

#include <cpgplot.h>

Pulsar::StokesPlotter::StokesPlotter ()
{
  plot_values  = "IQUV";
  plot_colours = "1234";
  plot_lines   = "1111";
  text_interface = new StokesPlotterTI (this);
}

void Pulsar::StokesPlotter::get_profiles (const Archive* data)
{
  profiles.resize( plot_values.size() );
  plot_sci.resize( plot_values.size() );
  plot_sls.resize( plot_values.size() );

  if (plot_values.size() > plot_colours.size())
    throw Error (InvalidState, "Pulsar::StokesPlotter::get_profiles",
		 "Mismatch: %u plots and %u colours",
		 plot_values.size(), plot_colours.size());

  if (plot_values.size() > plot_lines.size())
    throw Error (InvalidState, "Pulsar::StokesPlotter::get_profiles",
		 "Mismatch: %u plots and %u lines",
		 plot_values.size(), plot_lines.size());

  Reference::To<const PolnProfile> profile = new_Stokes (data, isubint, ichan);

  for (unsigned ipol=0; ipol < profiles.size(); ipol++) {
    profiles[ipol] = new_Profile (profile, plot_values[ipol]);
    plot_sci[ipol] = plot_colours[ipol] - '0';
    plot_sls[ipol] = plot_lines[ipol] - '0';
  }
}

