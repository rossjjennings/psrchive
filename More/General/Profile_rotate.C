/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::rotate
//

#include "Pulsar/Profile.h"
#include "FTransform.h"

#include <memory>
#include <math.h>

using namespace std;

bool Pulsar::Profile::rotate_in_phase_domain = false;

/*!  
  Rotate the profile by the specified phase.  The profile will be
  rotated such that the power at phase will be found at phase zero. ie.

  \f$t^\prime=t+\phi P\f$

  where \f$t^\prime\f$ is the new start time (rising edge of bin 0),
  \f$t\f$ is the original start time, \f$\phi\f$ is equal to phase,
  and \f$P\f$ is the period at the time of folding.
*/
void Pulsar::Profile::rotate_phase (double phase)
{
  if (verbose)
    cerr << "Pulsar::Profile::rotate phase=" << phase 
	 << " nbin=" << nbin << endl;

  if (phase == 0.0)
    return;

  // Ensure that phase runs from 0 to 1.  This step may help to
  // preserve precision if both phase and nbin are large.
  phase -= floor (phase);

  if (!rotate_in_phase_domain) {
    FTransform::shift (nbin, amps, phase*double(nbin));
  }
  else {

    // after using floor as above, phase is always greater than zero
    unsigned binshift = unsigned (phase * double(nbin) + 0.5);

    if (verbose)
      cerr << "Pulsar::Profile::rotate " << binshift << " bins" << endl;

    float* temp = new float[binshift];
    memcpy (temp, amps, binshift*sizeof(float));
    memmove (amps, amps+binshift, (nbin-binshift)*sizeof(float));
    memcpy (amps+nbin-binshift, temp, binshift*sizeof(float));
    delete [] temp;

  }

}
