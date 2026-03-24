/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"
#include "Pulsar/DispersionHistory.h"
#include "Pulsar/BirefringenceHistory.h"

using namespace std;

void Pulsar::Integration::update_absolute_dispersion () try
{
  if (verbose)
    cerr << "Pulsar::Integration::update_absolute_dispersion" << endl;

  auto aux = get<AuxColdPlasmaMeasures> ();

  if (!aux)
  {
    if (verbose)
      cerr << "Pulsar::Integration::update_absolute_dispersion no AuxColdPlasmaMeasures" << endl;
    return;
  }

  double aux_dm = aux->get_dispersion_measure();

  if (verbose)
    cerr << "Pulsar::Integration::update_absolute_dispersion aux:dm=" << aux_dm << endl;
  
  if (aux_dm != 0.0)
  {   
    auto history = getadd<DispersionHistory>();
    history->get_absolute()->set_corrected(true);
    history->get_absolute()->set_measure(aux_dm);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::update_absolute_dispersion";
}

void Pulsar::Integration::update_absolute_rotation () try
{
  if (verbose)
    cerr << "Pulsar::Integration::update_absolute_rotation" << endl;

  auto aux = get<AuxColdPlasmaMeasures> ();

  if (!aux)
  {
    if (verbose)
      cerr << "Pulsar::Integration::update_absolute_rotation no AuxColdPlasmaMeasures" << endl;
    return;
  }

  double aux_rm = aux->get_rotation_measure();

  if (verbose)
    cerr << "Pulsar::Integration::update_absolute_rotation aux:rm=" << aux_rm << endl;

  if (aux_rm != 0.0)
  {
    auto history = getadd<BirefringenceHistory>();
    history->get_absolute()->set_corrected(true);
    history->get_absolute()->set_measure(aux_rm);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::update_absolute_rotation";
}