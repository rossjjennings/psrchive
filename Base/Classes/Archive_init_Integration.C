/***************************************************************************
 *
 *   Copyright (C) 2006-2026 by Willem van Straten
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

void Pulsar::Archive::init_Dedisperse (Integration* subint, bool overwrite_absolute)
{
  bool has_prior_history = subint->get<Dedisperse>() != nullptr;
  auto history = subint->getadd<Dedisperse>();

  // start with the assumption that nothing is corrected
  history->get_relative()->set_corrected(false);

  if ( get_dedispersed() )
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Dedisperse dedispersed DM=" << get_dispersion_measure() << endl;
    history->get_relative()->set_corrected(true);
    history->get_relative()->set_reference_frequency( get_centre_frequency() );
    history->get_relative()->set_measure( get_dispersion_measure() );
  }

  if (has_prior_history && !overwrite_absolute)
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Dedisperse preserving existing absolute correction history" << endl;
    return;
  }

  // start with the assumption that nothing is corrected
  history->get_absolute()->set_corrected(false);

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

      history->get_absolute()->set_corrected(true);
      history->get_absolute()->set_measure( subaux->get_dispersion_measure() );
    }
  }
}

void Pulsar::Archive::init_DeFaraday (Integration* subint, bool overwrite_absolute)
{
  bool has_prior_history = subint->get<DeFaraday>() != nullptr;
  auto history = subint->getadd<DeFaraday>();

  // start with the assumption that nothing is corrected
  history->get_relative()->set_corrected(false);

  if ( get_dedispersed() )
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_DeFaraday derotated RM=" << get_rotation_measure() << endl;
    history->get_relative()->set_corrected(true);
    history->get_relative()->set_reference_frequency( get_centre_frequency() );
    history->get_relative()->set_measure( get_rotation_measure() );
  }

  if (has_prior_history && !overwrite_absolute)
  {
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_DeFaraday preserving existing absolute correction history" << endl;
    return;
  }

  // start with the assumption that nothing is corrected
  history->get_absolute()->set_corrected(false);

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
 
      auto history = subint->getadd<DeFaraday>();
      history->get_absolute()->set_corrected(true);
      history->get_absolute()->set_measure( subaux->get_rotation_measure() );
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

  /*
    bugs/509 - part 1
    Overwrite the absolute correction history only if the Integration does not already own such history.
    This is to stop an Archive from over-writing important history when it adopts an Integration
    from another source (note that Archive::use_Integration calls Archive::init_Integration).
    To ensure that history is initialized correctly when loading, Archive::init_Dedisperse
    and Archive::init_DeFaraday are called in Archive::load_Integration with overwrite_absolute = true
  */

  bool overwrite_absolute = false;
  init_Dedisperse (subint, overwrite_absolute);
  init_DeFaraday (subint, overwrite_absolute);

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
