/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveTemplates.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

void Pulsar::foreach (Archive* archive,
		       Reference::To< Transformation<Profile> > xform)
{
  for (unsigned i=0; i < archive->get_nsubint(); i++)
    foreach( archive->get_Integration(i), xform );
}

void Pulsar::foreach (Integration* subint,
		       Reference::To< Transformation<Profile> > xform)
{
  for (unsigned ipol = 0; ipol < subint->get_npol(); ipol++)
    for (unsigned ichan = 0; ichan < subint->get_nchan(); ichan++)
      xform->transform( subint->get_Profile (ipol, ichan) );
}

