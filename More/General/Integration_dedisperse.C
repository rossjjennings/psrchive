#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Physical.h"

#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::dedisperse
//
/*!

  Phase rotate the profiles in each frequency channel in order to
  remove the dispersion delay with respect to the centre frequency

  \post All profiles will be aligned to the reference frequency 
  returned by Integration::get_centre_frequency.

*/
void Pulsar::Integration::dedisperse () try {

  if ( get_dedispersed() &&
       dedispersed_dispersion_measure == get_dispersion_measure() &&
       dedispersed_centre_frequency == get_centre_frequency() )
  {
    if (verbose)
      cerr << "Pulsar::Integration::dedisperse already corrected" << endl;

    return;
  }

  if ( get_duration() == 0 )  {
    if (verbose)
      cerr << "Pulsar::Integration::dedisperse no data" << endl;
    return;
  }

  double pfold = get_folding_period();
  if (pfold == 0)
    throw Error (InvalidState, "Pulsar::Integration::dedisperse",
		 "folding period unknown");

  double dm = get_dispersion_measure();
  double reference_frequency = get_centre_frequency ();

  double base = 0.0;

  if ( get_dedispersed() )  {

    // calculate the delay between the old and new reference frequency, if any
    base = dispersion_delay (dm, dedispersed_centre_frequency,
                             reference_frequency);

    if (verbose)
      cerr << "Pulsar:Integration:::dedisperse delta frequency delay=" 
	   << base*1e3 << " ms" << endl;

    // set the effective dispersion measure to the difference b/w old and new
    dm -= dedispersed_dispersion_measure;

  }

  if (verbose)
    cerr << "Integration::dedisperse effective DM=" << dm
	 << " reference frequency=" << reference_frequency << endl;

  dedisperse (0, get_nchan(), dm, reference_frequency, base);

  dedispersed_dispersion_measure = get_dispersion_measure();
  dedispersed_centre_frequency = get_centre_frequency();

}
catch (Error& error) {
  throw error += "Pulsar::Integration::dedisperse";
}

/*! This worker function performs dedispersion without asking many
  questions.

   \param ichan the first channel to be dedispersed
   \param kchan the last channel to be dedispersed
   \param dm the dispersion measure 
   \param f0 the reference frequency 
   \param t0 time delay to be applied in addition to dispersion delay */

void Pulsar::Integration::dedisperse (unsigned ichan, unsigned kchan,
                                      double dm, double f0, double t0)
try {

  if (verbose)
    cerr << "Integration::dedisperse dm=" << dm << " f0=" << f0 
         << " t0=" << t0 << endl;

  if (dm == 0 && t0 == 0)
    return;

  double pfold = get_folding_period();

  if (pfold == 0)
    throw Error (InvalidState, "Pulsar::Integration::dedisperse",
                 "folding period unknown");

  if (ichan >= get_nchan())
    throw Error (InvalidRange, "Integration::dedisperse",
		 "start chan=%d >= nchan=%d", ichan, get_nchan());

  if (kchan > get_nchan())
    throw Error (InvalidRange, "Integration::dedisperse",
                 "end chan=%d > nchan=%d", kchan, get_nchan());

  for (unsigned jchan=ichan; jchan < kchan; jchan++) {

    double frequency = get_centre_frequency (jchan);
    double delay = t0 + dispersion_delay (dm, f0, frequency);

    for (unsigned ipol=0; ipol < get_npol(); ipol++)
      profiles[ipol][jchan] -> rotate (delay / pfold);

  }

}
catch (Error& error) {
  throw error += "Pulsar::Integration::dedisperse [private]";
}

