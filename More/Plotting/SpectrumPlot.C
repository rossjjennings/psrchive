#include "Pulsar/SpectrumPlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

void Pulsar::SpectrumPlot::get_spectra (const Archive* data)
{
  unsigned nchan = data->get_nchan();

  spectra.resize(1);
  spectra[0].resize(nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (data->get_Profile (isubint, ipol, ichan) -> get_weight() == 0.0)
      spectra[0][ichan] = 0.0;
    else
      spectra[0][ichan] = data->get_Profile (isubint, ipol, ichan) -> sum();
}

