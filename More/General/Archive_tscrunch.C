#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

bool Pulsar::Archive::weight_by_duration = true;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::fscrunch
//
/*!
  \param nscrunch number of neighbouring Integrations to add. 
                  If nscrunch == 0, then add all Integrations together
 */
void Pulsar::Archive::tscrunch (unsigned nscrunch)
{
  unsigned nsub = get_nsubint();

  if (nscrunch == 1 || nsub < 2)
    return;

  // if nscrunch == 0, default is to scrunch all sub_ints
  if (nscrunch == 0)
    nscrunch = nsub;

  if (nsub % nscrunch)
    throw Error (InvalidRange, "Archive::tscrunch",
		 "nsubint=%d %% nscrunch=%d = %d",
		 nsub, nscrunch, nsub%nscrunch);

  unsigned newsub = nsub / nscrunch;

  if (verbose) cerr << "Archive::tscrunch - scrunching " 
		    << nsub << " Integrations by " << nscrunch << endl;

  try {

    for (unsigned isub=0; isub < newsub; isub++) {

      if (verbose) cerr << "Archive::tscrunch resulting subint " 
			<< isub+1 << "/" << newsub << endl;

      unsigned start = isub * nscrunch;

      for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

	if (verbose) cerr << "Archive::tscrunch weighted_frequency chan="
			  << ichan << endl;

	double cfreq = weighted_frequency (ichan, start, start+nscrunch);

	if (verbose) 
	  cerr << "Archive::tscrunch dedisperse cfreq=" << cfreq << endl;

	for (unsigned iadd=0; iadd < nscrunch; iadd++)
	  get_Integration(start+iadd) -> dedisperse (cfreq, ichan);

	if (verbose) 
	  cerr <<  "Archive::tscrunch sum profiles" << endl;
	
	for (unsigned ipol=0; ipol < get_npol(); ++ipol) {

	  Profile* avg = get_Profile (isub, ipol, ichan);
	  Profile* add = get_Profile (start, ipol, ichan);

	  *(avg) = *(add);

	  for (unsigned jsub=1; jsub<nscrunch; jsub++) {
	    add = get_Profile (start+jsub, ipol, ichan);
	    *(avg) += *(add);
	  }

	} // for each poln
      } // for each channel
    } // for each integrated result
  } // end try block

  catch (Error& err) {
    throw err += "Archive::tscrunch";
  }

  for (unsigned isub=0; isub < newsub; isub++) {

    unsigned start = isub * nscrunch;

    MJD    mjd;
    double duration = 0.0;

    for (unsigned iadd=0; iadd < nscrunch; iadd++) {

      Integration* cur = get_Integration (start+iadd);

      duration += cur->get_duration();
      mjd      += cur->get_epoch();

    }

    get_Integration(isub) -> set_duration (duration);

    mjd /= double (nscrunch);

    // ensure that the polyco includes the new integration time
    update_model (mjd);

    if (get_type() == Signal::Pulsar) {

      // get the time of the first subint to be integrated into isub
      MJD firstmjd = get_Integration (isub * nscrunch) -> get_epoch ();
      // get the period at the time of the first subint
      double first_period = model.period(firstmjd);
      // get the phase at the time of the first subint
      Phase first_phase = model.phase(firstmjd);
      
      // get the phase at the midtime of the result
      Phase mid_phase = model.phase (mjd); 

      // calculate the phase difference
      Phase dphase = mid_phase - first_phase;

      // Subtract one period times phase difference from mjd      
      mjd -= dphase.fracturns() * first_period;

      get_Integration (isub)->set_epoch (mjd);
      get_Integration (isub)->set_folding_period (model.period(mjd));
      
      // The original code did not include the number of 
      // integer turns when computing the shift_time
    }
  }

  resize (newsub);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, to nearest kHz)
  \param  subint_start the first subint included in the calculation
  \param  subint_end one more than the index of the last subint
*/
double Pulsar::Archive::weighted_frequency (unsigned ichan,
					    unsigned start, unsigned end) 
  const
{
  unsigned nsubint = get_nsubint();

  if (end == 0)
    end = nsubint;

  // for now, ignore poln
  unsigned ipol = 0;

  if (nsubint == 0)
    throw Error (InvalidRange, "Archive::weighted_frequency", "nsubint == 0");

  if (start >= nsubint)
    throw Error (InvalidRange, "Archive::weighted_frequency",
		 "start=%d nsubint=%d", start, nsubint);

  if (end > nsubint)
    throw Error (InvalidRange, "Archive::weighted_frequency",
		 "end=%d nsubint=%d", end, nsubint);

  double weightsum = 0.0;
  double freqsum = 0.0;

  double fstart = 0.0;
  double fend = 0.0;

  try {
    for (unsigned isubint=start; isubint < end; isubint++) {
      
      const Profile* prof = get_Profile (isubint, ipol, ichan);
      
      double freq   = prof->get_centre_frequency();
      double weight = prof->get_weight();
      
      //if (verbose)
      //cerr << "Archive::weighted_frequency [" << isubint << "]"
      //  " freq=" << freq << " wt=" << weight << endl;

      if (weight_by_duration)
	weight *= get_Integration (isubint) -> get_duration();
      
      freqsum += freq * weight;
      weightsum += weight;
      
      if (isubint == start)
	fstart = freq;
      if (isubint == end-1)
	fend = freq;
    }
  }
  catch (Error& err) {
    throw err += "Archive::weighted_frequency";
  }
  
  double result = 0.0;
  
  if (weightsum != 0.0)
    result = freqsum / weightsum;
  else
    result = 0.5 * ( fstart + fend );
  
  // Nearest kHz
  result = 1e-3 * double( int(result*1e3) );
  return result;
}
