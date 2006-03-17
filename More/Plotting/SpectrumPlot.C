/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/SpectrumPlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

void Pulsar::SpectrumPlot::get_spectra (const Archive* data)
{
  unsigned nchan = data->get_nchan();

  spectra.resize(1);
  spectra[0].resize(nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {
    Reference::To<const Profile> profile;
    profile = get_Profile (data, isubint, ipol, ichan);
    if (profile -> get_weight() == 0.0)
      spectra[0][ichan] = 0.0;
    else if (ibin.get_integrate())
      spectra[0][ichan] = profile->sum();
    else
      spectra[0][ichan] = profile->get_amps()[ibin.get_value()];
  }
}
