/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

// Extension classes used to store state information
#include "Pulsar/AuxColdPlasma.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"
#include "Pulsar/Dedisperse.h"
#include "Pulsar/DeFaraday.h"

#include <iostream>

using namespace Pulsar;
using namespace std;

Pulsar::Integration* Pulsar::Archive::use_Integration (Integration* subint)
{
  init_Integration (subint);
  return subint;
}

void Pulsar::Archive::init_Dedisperse (Integration* subint)
{
  auto corrected = subint->getadd<Dedisperse>();

  // start with the assumption that nothing is corrected
  corrected->get_relative()->set_corrected(false);
  corrected->get_absolute()->set_corrected(false);

  if ( get_dedispersed() )
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Dedisperse dedispersed DM=" << get_dispersion_measure() << endl;
    corrected->get_relative()->set_corrected(true);
    corrected->get_relative()->set_reference_frequency( get_centre_frequency() );
    corrected->get_relative()->set_measure( get_dispersion_measure() );
  }

  auto subaux = subint->get<AuxColdPlasmaMeasures>();

  if (subaux)
  {
    auto aux = get<AuxColdPlasma>();
    if (!aux)
      throw Error (InvalidState, "Pulsar::Archive::init_Dedisperse",
                   "Integration has AuxColdPlasmaMeasures extension but Archive does not have AuxColdPlasma extension");

    if (aux->get_dispersion_corrected())
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::init_Dedisperse absolute dedispersed DM=" << subaux->get_dispersion_measure() << endl;

      corrected->get_absolute()->set_corrected(true);
      corrected->get_absolute()->set_measure( subaux->get_dispersion_measure() );
    }
  }
}

void Pulsar::Archive::init_DeFaraday (Integration* subint)
{
  auto corrected = subint->getadd<Dedisperse>();

  // start with the assumption that nothing is corrected
  corrected->get_relative()->set_corrected(false);
  corrected->get_absolute()->set_corrected(false);

  if ( get_dedispersed() )
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_DeFaraday derotated RM=" << get_rotation_measure() << endl;
    auto corrected = subint->getadd<DeFaraday>();
    corrected->get_relative()->set_corrected(true);
    corrected->get_relative()->set_reference_frequency( get_centre_frequency() );
    corrected->get_relative()->set_measure( get_rotation_measure() );
  }

  auto subaux = subint->get<AuxColdPlasmaMeasures>();

  if (subaux)
  {
    auto aux = get<AuxColdPlasma>();
    if (!aux)
      throw Error (InvalidState, "Pulsar::Archive::init_DeFaraday",
                   "Integration has AuxColdPlasmaMeasures extension but Archive does not have AuxColdPlasma extension");

    if (aux->get_birefringence_corrected())
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::init_DeFaraday absolute derotated RM=" << subaux->get_rotation_measure() << endl;
 
      auto corrected = subint->getadd<DeFaraday>();
      corrected->get_absolute()->set_corrected(true);
      corrected->get_absolute()->set_measure( subaux->get_rotation_measure() );
    }
  }  
}

/*!  
  After an Integration has been loaded from disk, this method
  ensures that various internal book-keeping attributes are
  initialized.
*/
void Pulsar::Archive::init_Integration (Integration* subint, bool check_phase)
{
  if (verbose > 2)
    cerr << "Pulsar::Archive::init_Integration check_phase=" << check_phase << endl;

  subint->parent = this;

  // Add and initialize a Dedisperse extension only if the Integration does not already own one
  {
    auto corrected = subint->get<Dedisperse>();
    if (!corrected)
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::init_Integration initializing Dedisperse extension" << endl;
      init_Dedisperse (subint);
    }
    else if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration Integration already has Dedisperse extension" << endl;
  }

  // Add and initialize a DeFaraday extension only if the Integration does not already own one
  {
    auto corrected = subint->get<DeFaraday>();
    if (!corrected)
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::init_Integration initializing DeFaraday extension" << endl;
      init_DeFaraday (subint);
    }
    else if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration Integration already has DeFaraday extension" << endl;
  }

  subint->zero_phase_aligned = false;

  if (check_phase && model)
  {
    MJD epoch = subint->get_epoch();
    Phase phase = model->phase( epoch );

    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration epoch="
           << epoch.printdays(20) << " fturn=" << phase.fracturns() << endl;

    double frac = phase.fracturns();
    frac = fabs( frac - round(frac) );

    subint->zero_phase_aligned = frac < 1e-8;

    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration frac=" << frac
           << " aligned=" << subint->zero_phase_aligned << endl;
  }
}
