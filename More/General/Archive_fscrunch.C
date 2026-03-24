/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/AuxColdPlasma.h"

using namespace std;

/*!
  Simply calls Integration::fscrunch for each Integration
  \param nscrunch the number of frequency channels to add together
 */
void Pulsar::Archive::fscrunch (unsigned nscrunch)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
  {
    auto subint = get_Integration(isub);
    subint->fscrunch (nscrunch);
  }

  set_nchan (get_Integration(0)->get_nchan());
  update_absolute_dispersion();
  update_absolute_rotation();
}

/*!
  Useful wrapper for Archive::fscrunch
*/
void Pulsar::Archive::fscrunch_to_nchan (unsigned new_chan)
{
  if (new_chan == 0)
    throw Error (InvalidParam, "Pulsar::Archive::fscrunch_to_nchan",
                               "requested nchan=0");

  if (get_nchan() % new_chan)
    throw Error (InvalidParam, "Pulsar::Archive::fscrunch_to_nchan",
		             "requested nchan=%u modulo current nchan=%u is non-zero",
                 new_chan, get_nchan());

  fscrunch(get_nchan() / new_chan);
}

