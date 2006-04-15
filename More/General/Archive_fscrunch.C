/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  Simply calls Integration::fscrunch for each Integration
  \param nscrunch the number of frequency channels to add together
 */
void Pulsar::Archive::fscrunch (unsigned nscrunch)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> fscrunch (nscrunch);

  set_nchan (get_Integration(0)->get_nchan());
}

/*!
  Useful wrapper for Archive::fscrunch
*/
void Pulsar::Archive::fscrunch_to_nchan (unsigned new_chan)
{
  if (get_nchan() % new_chan != 0)
    throw Error (InvalidParam, "Pulsar::Archive::fscrunch_to_nchan",
		 "Invalid nchan request");
  else if (get_nchan() < new_chan)
    throw Error (InvalidParam, "Pulsar::Archive::fscrunch_to_nchan",
		 "Archive has too few channels");
  else
    fscrunch(get_nchan() / new_chan);
}

