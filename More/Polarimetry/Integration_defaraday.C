/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Integration.h"
#include "Pulsar/FaradayRotation.h"

static Pulsar::FaradayRotation xform;

/*! 
  Calls FaradayRotation::transform
*/
void Pulsar::Integration::defaraday () try {

  xform.transform (this);

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
   \param r0 transformation to be applied in addition to faraday rotation */

void Pulsar::Integration::defaraday (unsigned ichan, unsigned kchan,
                                     double rm, double f0)
try {

  xform.set_rotation_measure( rm );
  xform.set_reference_frequency( f0 );

  // ensure that delta is equal to the identity matrix
  xform.set_delta( 1.0 );

  xform.execute (this, ichan, kchan);

}
catch (Error& error) {
  throw error += "Pulsar::Integration::defaraday [private]";
}
