/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Profile.h"
#include "Error.h"

/*!
  Calculates the shift between observed and template profiles.
  Returns a basic Tempo::toa object
*/

Tempo::toa Pulsar::Profile::toa (const Profile& std, const MJD& mjd,
				 double period, char nsite, string arguments,
				 Tempo::toa::Format fmt) const
{
  Tempo::toa retval (fmt);

  float  ephase;
  double phase;
  
  Estimate<double> Ephase = shift (std);
  phase = Ephase.val;
  ephase = sqrt(Ephase.var);

  if (verbose) {
    cerr << "Pulsar::Profile::toa phase shift = " << phase << endl;
    cerr << "Pulsar::Profile::toa phase error = " << ephase << endl;
  }

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + phase * period);
  retval.set_error     (ephase * period * 1e6);

  retval.set_telescope (nsite);
  retval.set_auxilliary_text(arguments);
  
  if (verbose) {
    fprintf (stderr, "Pulsar::Profile::toa created:\n");
    retval.unload (stderr);
  }
  return retval;
}
