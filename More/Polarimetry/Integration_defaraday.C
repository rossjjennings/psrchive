#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Calibration/Faraday.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::defaraday
//
/*! 

  Rotate the polarization profiles in each frequency channel about the
  Stokes V axis in order to remove Faraday rotation with respect to
  the centre frequency.

  \pre The noise contribution to Stokes Q and U should have been removed.
  \pre The archive must have full polarimetric information.
  \post All profiles will have a position angle aligned to the
  reference frequency returned by Integration::get_centre_frequency.

*/
void Pulsar::Integration::defaraday () try {

  if ( get_faraday_corrected() &&
       defaradayed_rotation_measure == get_rotation_measure() &&
       defaradayed_centre_frequency == get_centre_frequency() )
  {
    if (verbose)
      cerr << "Pulsar::Integration::defaraday already corrected" << endl;

    return;
  }

  double rm = get_rotation_measure();
  double reference_frequency = get_centre_frequency();

  Jones<double> base = 1;

  if ( get_faraday_corrected() )  {

    Calibration::Faraday faraday;

    // calculate the rotation arising from the new centre frequency, if any
    faraday.set_reference_frequency( defaradayed_centre_frequency );
    faraday.set_frequency( reference_frequency );
    faraday.set_rotation_measure( rm );

    base = faraday.evaluate();

    // set the effective rotation measure to the difference
    rm -= defaradayed_rotation_measure;

  }

  if (verbose)
    cerr << "Integration::dedisperse effective RM=" << rm
	 << " reference frequency=" << reference_frequency << endl;

  defaraday (0, get_nchan(), rm, reference_frequency, base);

  defaradayed_rotation_measure = get_rotation_measure();
  defaradayed_centre_frequency = get_centre_frequency();

}
catch (Error& error) {
  throw error += "Pulsar::Integration::defaraday";
}


/*! This worker function corrects faraday rotation without asking many
  questions.

   \param ichan the first channel to be defaradayd
   \param kchan the last channel to be defaradayd
   \param rm the rotation measure
   \param f0 the reference frequency
   \param r0 transformation to be applied in addition to faraday rotation */

void Pulsar::Integration::defaraday (unsigned ichan, unsigned kchan,
                                     double rm, double f0, 
                                     const Jones<double>& r0)
{
  if (verbose)
    cerr << "Pulsar::Integration::defaraday rm=" << rm << " f0=" << f0
         << " r0=" << r0 << endl;

  if (rm == 0 && r0 == 1)
    return;

  if (ichan >= get_nchan())
    throw Error (InvalidRange, "Pulsar::Integration::defaraday",
                 "start chan=%d >= nchan=%d", ichan, get_nchan());

  if (kchan > get_nchan())
    throw Error (InvalidRange, "Pulsar::Integration::defaraday",
                 "end chan=%d > nchan=%d", kchan, get_nchan());

  Calibration::Faraday faraday;

  faraday.set_reference_frequency( f0 );
  faraday.set_rotation_measure( rm );

  for (unsigned jchan=ichan; jchan < kchan; jchan++) {

    faraday.set_frequency( get_centre_frequency (jchan) );

    Reference::To<PolnProfile> poln_profile = new_PolnProfile (jchan);

    poln_profile->transform (inv( r0 * faraday.evaluate() ));

  }

}
