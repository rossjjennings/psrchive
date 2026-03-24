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

void Pulsar::foreach (Archive* archive, const Archive* operand,
                      Reference::To< Combination<Profile> > xform)
{
  // allow one-to-many combinations
  unsigned archive_nsubint = archive->get_nsubint();
  unsigned operand_nsubint = operand->get_nsubint();
  unsigned max_nsubint = std::max(archive_nsubint, operand_nsubint);

  for (unsigned isubint=0; isubint < max_nsubint; isubint++)
  {
    unsigned archive_isubint = (archive_nsubint == 1) ? 0 : isubint;
    unsigned operand_isubint = (operand_nsubint == 1) ? 0 : isubint;

    foreach( archive->get_Integration(archive_isubint), 
             operand->get_Integration(operand_isubint), xform );
  }
}

void Pulsar::foreach (Integration* subint, const Integration* operand,
                      Reference::To< Combination<Profile> > xform)
{
  // allow one-to-many combinations
  unsigned subint_nchan = subint->get_nchan();
  unsigned operand_nchan = operand->get_nchan();
  unsigned max_nchan = std::max(subint_nchan, operand_nchan);

  for (unsigned ipol = 0; ipol < subint->get_npol(); ipol++)
  {
    for (unsigned ichan = 0; ichan < subint->get_nchan(); ichan++)
    {
      unsigned subint_ichan = (subint_nchan == 1) ? 0 : ichan;
      unsigned operand_ichan = (operand_nchan == 1) ? 0 : ichan;

      xform->set_operand( operand->get_Profile (ipol, operand_ichan) );
      xform->transform( subint->get_Profile (ipol, subint_ichan) );
    }
  }
}

