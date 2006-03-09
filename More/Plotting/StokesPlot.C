#include "Pulsar/StokesPlotterTI.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
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

  const Integration* subint = data->get_Integration (isubint);
  Reference::To<const PolnProfile> profile = subint->new_PolnProfile(ichan);

  if (profile->get_state() != Signal::Stokes) {
    Reference::To<PolnProfile> clone = profile->clone();
    clone->convert_state(Signal::Stokes);
    profile = clone;
  }

  for (unsigned ipol=0; ipol < profiles.size(); ipol++) {
    profiles[ipol] = get_Profile (profile, plot_values[ipol]);
    plot_sci[ipol] = plot_colours[ipol] - '0';
    plot_sls[ipol] = plot_lines[ipol] - '0';
  }
}

const Pulsar::Profile*
Pulsar::StokesPlotter::get_Profile (const PolnProfile* data, char code)
{
  Reference::To<Profile> profile;

  switch (code) {
  case 'I':
    return data->get_Profile(0);
  case 'Q':
    return data->get_Profile(1);
  case 'U':
    return data->get_Profile(2);
  case 'V':
    return data->get_Profile(3);

  case 'L': {
    profile = new Profile;
    data->get_linear(profile);
    return profile.release();
  }

  case 'p': {
    profile = new Profile;
    data->get_polarized(profile);
    return profile.release();
  }

  default:
    throw Error (InvalidParam, "Pulsar::StokesPlotter::get_Profile",
		 "Unknown polarization code '%c'", code);
  }

}

