/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Predictor.h"

// Integration Extension classes used to store state information
#include "Pulsar/DeFaraday.h"
#include "Pulsar/Dedisperse.h"

#include <iostream>
using namespace std;

/*!  
  After an Integration has been loaded from disk, this method
  ensures that various internal book-keeping attributes are
  initialized.
*/
void Pulsar::Archive::init_Integration (Integration* subint, bool check_phase)
{
  subint->archive = this;

  if ( get_dedispersed() ) {
    Dedisperse* corrected = new Dedisperse;
    corrected->set_reference_frequency( get_centre_frequency() );
    corrected->set_dispersion_measure( get_dispersion_measure() );
    subint->add_extension( corrected );
  }

  if ( get_faraday_corrected() ) {
    DeFaraday* corrected = new DeFaraday;
    corrected->set_reference_frequency( get_centre_frequency() );
    corrected->set_rotation_measure( get_rotation_measure() );
    subint->add_extension( corrected );
  }

  subint->zero_phase_aligned = false;

  if (check_phase && model) {
    MJD epoch = subint->get_epoch();
    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration epoch = " << epoch << endl;
    Phase phase = model->phase( epoch );

    double frac = std::min( fabs(phase.fracturns()),
			    fabs(1.0-phase.fracturns()) );

    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration phase = " << frac << endl;

    subint->zero_phase_aligned = frac < 1e-8;;
  }

}
