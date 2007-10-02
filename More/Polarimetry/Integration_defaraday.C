/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Integration.h"
#include "Pulsar/FaradayRotation.h"

#include <iostream>
using namespace std;

static Pulsar::FaradayRotation* xform = 0;

/*! 
  Calls FaradayRotation::transform
*/
void Pulsar::Integration::defaraday () try {

  if (verbose)
    cerr << "Pulsar::Integration::defaraday RM=" << get_rotation_measure()
	 << " fixed=" << get_faraday_corrected() << endl;

  if (!xform)
    xform = new FaradayRotation;

  xform->transform (this);

}
catch (Error& error) {
  throw error += "Pulsar::Integration::defaraday";
}


/*! This worker function corrects faraday rotation without asking many
  questions.

   \param ichan the first channel to be defaradayd
   \param kchan one greater than the last channel to be defaradayd
   \param rm the rotation measure
   \param f0 the reference frequency
*/
void Pulsar::Integration::defaraday (unsigned ichan, unsigned kchan,
                                     double rm, double f0)
try {

  if (!xform)
    xform = new FaradayRotation;

  xform->set_rotation_measure( rm );
  xform->set_reference_frequency( f0 );

  // ensure that delta is equal to the identity matrix
  xform->set_delta( 1.0 );

  xform->range (this, ichan, kchan);

}
catch (Error& error) {
  throw error += "Pulsar::Integration::defaraday [private]";
}
