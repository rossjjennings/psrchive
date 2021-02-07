/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SpectrumPlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/ArchiveStatistic.h"

using namespace std;

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::SpectrumPlot::SpectrumPlot ()
  : ibin (0, true) // sum over all phase bins by default
{
}

Pulsar::SpectrumPlot::~SpectrumPlot ()
{
}

//! Set the profile statistic
void Pulsar::SpectrumPlot::set_statistic (ArchiveStatistic* stat)
{
  statistic = stat;
}


//! Get the profile statistic
Pulsar::ArchiveStatistic* Pulsar::SpectrumPlot::get_statistic () const
{
  return statistic;
}

void Pulsar::SpectrumPlot::get_spectra (const Archive* data)
{
  unsigned nchan = data->get_nchan();

  spectra.resize(1);
  spectra[0].resize(nchan);

  Reference::To<ProfileStats> stats;
  Reference::To<TextInterface::Parser> parser;

  if (!expression.empty())
  {
    stats = new ProfileStats;
    parser = stats->get_interface ();
  }

  Reference::To<const Integration> subint;
  subint = get_Integration (data, isubint);

  if (statistic)
  {
    statistic->set_Archive (data);
    statistic->set_subint (isubint);
    statistic->set_pol (ipol);
  }

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    Reference::To<const Profile> profile;
    profile = get_Profile (subint, ipol, ichan);
    
    if (profile -> get_weight() == 0.0)
    {
      spectra[0][ichan] = 0.0;
      continue;
    }

    if (statistic)
    {
      statistic->set_chan (ichan);
      spectra[0][ichan] = statistic->get();
      continue;
    }

    if (stats)
    {
      stats->set_Profile (profile);
      string value = process( parser, expression );
      spectra[0][ichan] = fromstring<float>( value );
      continue;
    }
    
    if (ibin.get_integrate())
      spectra[0][ichan] = profile->sum() / (float)profile->get_nbin();
    else
      spectra[0][ichan] = profile->get_amps()[ibin.get_value()];
  }
}

//! Disable baseline removal
void Pulsar::SpectrumPlot::preprocess (Archive* archive)
{
}
