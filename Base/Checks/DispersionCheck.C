/***************************************************************************
 *
 *   Copyright (C) 2004-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DispersionCheck.h"
#include "Pulsar/DispersionHistory.h"
#include "Pulsar/AuxColdPlasma.h"

using namespace std;

// defined in DispersionCheck.C
bool diff (double x, double y);

void Pulsar::DispersionCheck::apply (const Archive* archive)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::DispersionCheck::apply verification" << endl;

  unsigned nsubint = archive->get_nsubint();

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    check_relative (archive, isubint);
    check_absolute (archive, isubint);
  }
}

void Pulsar::DispersionCheck::check_relative (const Archive* archive, unsigned isubint)
{
  const Integration* subint = archive->get_Integration (isubint);
  const DispersionHistory* ext = subint->get<DispersionHistory>();

  if (!archive->get_dedispersed())
  {
    if (ext && ext->get_relative()->get_corrected())
    throw Error (InvalidState, "Pulsar::DispersionCheck::apply",
                "Archive::dedispsersed is not set and Integration[%d]\n\t"
                "has a DispersionHistory Extension with the relative correction flag set", isubint);
  }
  else if (archive->get_dispersion_measure() != 0)
  {
    if (!ext)
        throw Error (InvalidState, "Pulsar::DispersionCheck::apply",
                "Archive::dedispsersed is set and Integration[%d]\n\t"
                "has no DispersionHistory Extension", isubint);

    if (!ext->get_relative()->get_corrected())
        throw Error (InvalidState, "Pulsar::DispersionCheck::apply",
                "Archive::dedispsersed is set and Integration[%d]\n\t"
                "has a DispersionHistory Extension without the relative correction flag set", isubint);

    if (diff( ext->get_relative()->get_reference_frequency(), archive->get_centre_frequency() ))
        throw Error (InvalidState, "Pulsar::DispersionCheck::apply",
                "Archive::dedispsersed is set and Integration[%d]\n\t"
                "DispersionHistory::reference_frequency = %lf doesn't equal\n\t"
                "Archive::centre_frequency = %lf", isubint,
                ext->get_relative()->get_reference_frequency(),
                archive->get_centre_frequency());

    if (diff( ext->get_relative()->get_measure(), archive->get_dispersion_measure() ))
        throw Error (InvalidState, "Pulsar::DispersionCheck::apply",
                "Archive::dedispsersed is set and Integration[%d]\n\t"
                "DispersionHistory::get_relative()->measure = %lf does not equal\n\t"
                "Archive::dispersion_measure = %lf", isubint,
                ext->get_relative()->get_measure(),
                archive->get_dispersion_measure());
  }
}

void Pulsar::DispersionCheck::check_absolute (const Archive* archive, unsigned isubint)
{
  if (Integration::verbose)
    cerr << "Pulsar::DispersionCheck::check_absolute isubint=" << isubint << endl;

  const Integration* subint = archive->get_Integration (isubint);
  const DispersionHistory* ext = subint->get<DispersionHistory>();

  bool integration_corrected = false;
  string integration_description;

  if (ext)
  {
    integration_description = "Integration has DispersionHistory extension";
    if (ext->get_absolute()->get_corrected())
    {
      integration_corrected = true;
      integration_description += " with absolute correction flag set";
    }
    else
    {
      integration_description += " without absolute correction flag set";
    }
  }
  else
  {
    integration_description = "Integration has no DispersionHistory extension";
  }

  if (Integration::verbose)
    cerr << "Pulsar::DispersionCheck::check_absolute Integration description='" << integration_description << "'" << endl;
  
  const AuxColdPlasma* aux = archive->get<AuxColdPlasma>();

  bool archive_corrected = false;
  string archive_description;

  if (aux)
  {
    archive_description = "Archive has AuxColdPlasma extension";
    if (aux->get_dispersion_corrected())
    {
      archive_corrected = true;
      archive_description += " with dispersion corrected flag set";
    }
    else
    {
      archive_description += " without dispersion corrected flag set";
    }
  }
  else
  {
    archive_description = "Archive has no AuxColdPlasma extension";
  }
  
  if (Integration::verbose)
    cerr << "Pulsar::DispersionCheck::check_absolute Archive description='" << archive_description << "'" << endl;

  if (archive_corrected != integration_corrected)
  {
    // special case: in old files, aux:dmc may equal 1 even when there is no int:aux:dm to correct
    if (!integration_corrected && ext->get_absolute()->get_measure() == 0.0)
    {
      if (Integration::verbose)
        cerr << "Pulsar::DispersionCheck::check_absolute allowing correction mismatch for 0 DM" << endl;
      return;
    }

    throw Error (InvalidState, "Pulsar::DispersionCheck::apply",
                 archive_description + "\n\tAND\n\t" + integration_description); 
  }
}
