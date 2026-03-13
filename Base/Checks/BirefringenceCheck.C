/***************************************************************************
 *
 *   Copyright (C) 2004-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BirefringenceCheck.h"
#include "Pulsar/BirefringenceHistory.h"
#include "Pulsar/AuxColdPlasma.h"

using namespace std;

bool diff (double x, double y)
{
  return fabs(x-y)/(x+y) > 1e-15;
}

void Pulsar::BirefringenceCheck::apply (const Archive* archive)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::BirefringenceCheck::apply verification" << endl;

  unsigned nsubint = archive->get_nsubint();

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    check_relative (archive, isubint);
    check_absolute (archive, isubint);
  }
}

void Pulsar::BirefringenceCheck::check_relative (const Archive* archive, unsigned isubint)
{
  const Integration* subint = archive->get_Integration (isubint);
  const BirefringenceHistory* ext = subint->get<BirefringenceHistory>();

  if (!archive->get_faraday_corrected())
  {
    if (ext && ext->get_relative()->get_corrected())
    throw Error (InvalidState, "Pulsar::BirefringenceCheck::apply",
                "Archive::faraday_corrected is not set and Integration[%d]\n\t"
                "has a BirefringenceHistory Extension with the relative correction flag set", isubint);
  }
  else
  {
    if (!ext)
        throw Error (InvalidState, "Pulsar::BirefringenceCheck::apply",
                "Archive::faraday_corrected is set and Integration[%d]\n\t"
                "has no BirefringenceHistory Extension", isubint);

    if (!ext->get_relative()->get_corrected())
        throw Error (InvalidState, "Pulsar::BirefringenceCheck::apply",
                "Archive::faraday_corrected is set and Integration[%d]\n\t"
                "has a BirefringenceHistory Extension without the relative correction flag set", isubint);

    if (diff( ext->get_relative()->get_reference_frequency(), archive->get_centre_frequency() ))
        throw Error (InvalidState, "Pulsar::BirefringenceCheck::apply",
                "Archive::faraday_corrected is set and Integration[%d]\n\t"
                "BirefringenceHistory::reference_frequency = %lf doesn't equal\n\t"
                "Archive::centre_frequency = %lf", isubint,
                ext->get_relative()->get_reference_frequency(),
                archive->get_centre_frequency());

    if (diff( ext->get_relative()->get_measure(), archive->get_rotation_measure() ))
        throw Error (InvalidState, "Pulsar::BirefringenceCheck::apply",
                "Archive::faraday_corrected is set and Integration[%d]\n\t"
                "BirefringenceHistory::get_relative()->measure = %lf does not equal\n\t"
                "Archive::rotation_measure = %lf", isubint,
                ext->get_relative()->get_measure(),
                archive->get_rotation_measure());
  }
}

void Pulsar::BirefringenceCheck::check_absolute (const Archive* archive, unsigned isubint)
{
  if (Integration::verbose)
    cerr << "Pulsar::BirefringenceCheck::check_absolute isubint=" << isubint << endl;

  const Integration* subint = archive->get_Integration (isubint);
  const BirefringenceHistory* ext = subint->get<BirefringenceHistory>();

  bool integration_corrected = false;
  string integration_description;

  if (ext)
  {
    integration_description = "Integration has BirefringenceHistory extension";
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
    integration_description = "Integration has no BirefringenceHistory extension";
  }

  if (Integration::verbose)
    cerr << "Pulsar::BirefringenceCheck::check_absolute Integration description='" << integration_description << "'" << endl;
  
  const AuxColdPlasma* aux = archive->get<AuxColdPlasma>();

  bool archive_corrected = false;
  string archive_description;

  if (aux)
  {
    archive_description = "Archive has AuxColdPlasma extension";
    if (aux->get_birefringence_corrected())
    {
      archive_corrected = true;
      archive_description += " with birefringence corrected flag set";
    }
    else
    {
      archive_description += " without birefringence corrected flag set";
    }
  }
  else
  {
    archive_description = "Archive has no AuxColdPlasma extension";
  }
  
  if (Integration::verbose)
    cerr << "Pulsar::BirefringenceCheck::check_absolute Archive description='" << archive_description << "'" << endl;

  if (archive_corrected != integration_corrected)
  {
    // special case: in old files, aux:rmc may equal 1 even when there is no int:aux:rm to correct
    if (!integration_corrected && (!ext || ext->get_absolute()->get_measure() == 0.0))
    {
      if (Integration::verbose)
      {
        cerr << "Pulsar::BirefringenceCheck::check_absolute allowing correction mismatch for integration ";
        if (!ext)
          cerr << "without aux:rm" << endl;
        else
          cerr << "with aux:rm=0" << endl;
      }

      return;
    }

    throw Error (InvalidState, "Pulsar::BirefringenceCheck::apply",
                 archive_description + "\n\tAND\n\t" + integration_description); 
  }
}
