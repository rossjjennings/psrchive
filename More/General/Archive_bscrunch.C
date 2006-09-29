/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  Useful wrapper for Archive::bscrunch
*/
void Pulsar::Archive::bscrunch_to_nbin (unsigned new_nbin)
{
  if (new_nbin <= 0)
    throw Error (InvalidParam, "Pulsar::Archive::bscrunch_to_nbin",
                 "Invalid nbin request");
  else if (get_nbin() < new_nbin)
    throw Error (InvalidParam, "Pulsar::Archive::bscrunch_to_nbin",
                 "Archive has too few bins (%d) to bscrunch to %d bins",
                 get_nbin(), new_nbin);
  else
    bscrunch(get_nbin() / new_nbin);
}

/*!
  Simply calls Integration::bscrunch for each Integration
  \param nscrunch the number of phase bins to add together
  */
void Pulsar::Archive::bscrunch (unsigned nscrunch)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> bscrunch (nscrunch);

  set_nbin (get_Integration(0)->get_nbin());
}


