#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::dedisperse
//
/*!
  Rotate the profiles in each chan in order to remove the dispersion delay.
  \param frequency the frequency (in MHz) to which the delay is referenced
  \pre the period (in seconds) at which the pulsar signal was folded must
       have been previously set using Integration::set_folding_period.
  \pre the dispersion measure (in \f${\rm pc cm}^{-3}\f$) must have been
       previously set using Integration::set_dispersion_measure.
*/
void Pulsar::Integration::dedisperse (double frequency)
{
  double dm = get_dispersion_measure();
  double pfold = get_folding_period();

  if (dm == 0)
    return;
  if (pfold == 0)
    return;

  if (frequency == 0.0)
    frequency = weighted_frequency ();

  if (verbose)
    cerr << "Integration::dedisperse DM=" << dm
	 <<" freq="<< frequency << endl;

  for (unsigned ipol=0; ipol < get_npol(); ipol++)
    for (unsigned ichan=0; ichan < get_nchan(); ichan++)
      profiles[ipol][ichan] -> dedisperse (dm, frequency, pfold);

}

void Pulsar::Integration::dedisperse (double frequency, unsigned chan)
{
  double dm = get_dispersion_measure();
  double pfold = get_folding_period();

  if (verbose)
    cerr << "Integration::dedisperse dm=" << dm << " pfold=" << pfold << endl;

  if (dm == 0)
    return;
  if (pfold == 0)
    return;

  if (chan < 0 || chan >= get_nchan())
    throw Error (InvalidRange, "Integration::dedisperse",
		 "chan=%d nchan=%d", chan, get_nchan());

  if (frequency == 0.0)
    throw Error (InvalidParam, "Integration::dedisperse",
		 "frequency == 0.0");

  if (verbose)
    cerr << "Integration::dedisperse chan=" 
	 << chan << " npol=" << get_npol() << endl;

  for (unsigned ipol=0; ipol < get_npol(); ipol++)
    profiles[ipol][chan] -> dedisperse (dm, frequency, pfold);
  
  return;
}

  // else, dedisperse the lot

