/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  The defaraday method corrects the Faraday rotation between
  each frequency channel and that of the reference frequency
  defined by get_centre_frequency.

  \pre The Archive must contain full polarimetric data
  \pre The noise contribution to Stokes Q and U should have been removed.
*/
void Pulsar::Archive::defaraday ()
{
  if (get_nsubint() == 0)
    return;

  if (!get_poln_calibrated() && verbose)
    cerr << "Pulsar::Archive::defaraday WARNING data not calibrated" << endl;

  if (!get_instrument_corrected() && verbose)
    cerr << "Pulsar::Archive::defaraday WARNING feed not corrected" << endl;

  for (unsigned i = 0; i < get_nsubint(); i++)
    get_Integration(i)->defaraday ();
  
  set_faraday_corrected (true);
}
