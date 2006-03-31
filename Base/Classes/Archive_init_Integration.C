/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

// Integration Extension classes used to store state information
#include "Pulsar/DeFaraday.h"
#include "Pulsar/Dedisperse.h"

/*!
  This method may be useful during load, as only the Archive base class
  has access to the Integration::archive attribute.
*/
void Pulsar::Archive::init_Integration (Integration* subint)
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
}
