/***************************************************************************
 *
 *   Copyright (C) 2025-2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/AuxColdPlasma.h"
#include "Pulsar/BirefringenceHistory.h"
#include "Pulsar/DispersionHistory.h"

using namespace std;

void Pulsar::Archive::update_absolute_dispersion () try
{
  if (verbose > 1)
    cerr << "Pulsar::Archive::update_absolute_dispersion" << endl;

  bool absolute_dm_corrected = false;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
  {
    auto subint = get_Integration(isub);
    auto history = subint->get<DispersionHistory>();
    if (!history)
      continue;
    if (history->get_absolute()->get_corrected())
      absolute_dm_corrected = true;
  }

  if (absolute_dm_corrected)
  {
    auto aux = getadd<AuxColdPlasma>();
    aux->set_dispersion_corrected(true);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::update_absolute_dispersion";
}

void Pulsar::Archive::update_absolute_rotation () try
{
  if (verbose > 1)
    cerr << "Pulsar::Archive::update_absolute_rotation" << endl;

  bool absolute_rm_corrected = false;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
  {
    auto subint = get_Integration(isub);
    auto history = subint->get<BirefringenceHistory>();
    if (!history)
      continue;
    if (history->get_absolute()->get_corrected())
      absolute_rm_corrected = true;
  }

  if (absolute_rm_corrected)
  {
    auto aux = getadd<AuxColdPlasma>();
    aux->set_birefringence_corrected(true);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::update_absolute_rotation";
}