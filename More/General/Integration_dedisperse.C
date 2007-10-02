/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/Dispersion.h"

static Pulsar::Dispersion xform;

/*! 
  Calls Dispersion::transform
*/
void Pulsar::Integration::dedisperse () try {

  xform.transform (this);

}
catch (Error& error) {
  throw error += "Pulsar::Integration::dedisperse";
}


/*! This worker function corrects dispersion delays without asking
  many questions.

   \param ichan the first channel to be dedispersed
   \param kchan one greater than the last channel to be dedispersed
   \param rm the dispersion measure
   \param f0 the reference frequency
*/
void Pulsar::Integration::dedisperse (unsigned ichan, unsigned kchan,
				      double rm, double f0)
try {

  xform.set_dispersion_measure( rm );
  xform.set_reference_frequency( f0 );

  // ensure that delta is equal to zero
  xform.set_delta( 0.0 );

  xform.range (this, ichan, kchan);

}
catch (Error& error) {
  throw error += "Pulsar::Integration::dedisperse [private]";
}
