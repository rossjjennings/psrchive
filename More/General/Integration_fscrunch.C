#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::fscrunch
//
/*!

  \param nscrunch number of neighbouring frequency channels to
  integrate; if zero, then all channels are integrated into one

  Before integrating the frequency channels:
  <UL>
  <LI> If the data have not already been dedispersed, then for each
  resulting frequency channel, a weighted centre frequency will be
  calculated and dispersion delays between this reference frequency
  and the individual channel centre frequencies will be removed.

  <LI> If the data have not already been corrected for Faraday
  rotation, then for each resulting frequency channel, a weighted
  centre frequency will be calculated and Faraday rotation between
  this reference frequency and the individual channel centre
  frequencies will be corrected.
  </UL>
 */
void Pulsar::Integration::fscrunch (unsigned nscrunch)
{
  if (verbose)
    cerr << "Pulsar::Integration::fscrunch"
      " nadd=" << nscrunch << " dm=" << get_dispersion_measure() << endl;

  if (nscrunch == 1 || get_nchan() == 1)
    // nothing to scrunch
    return;

  if (nscrunch == 0)
    nscrunch = get_nchan();

  if (get_nchan() % nscrunch)
    throw Error (InvalidRange, "Pulsar::Integration::fscrunch",
		 "nchan=%d %% nscrunch=%d = %d",
		 get_nchan(), nscrunch, get_nchan()%nscrunch);

  unsigned newchan = get_nchan()/nscrunch;

  double dm = get_dispersion_measure();
  bool must_dedisperse = dm != 0 && !get_dedispersed();

  double rm = get_rotation_measure();
  bool must_defaraday = rm != 0 && !get_faraday_corrected() && get_npol() == 4;

  for (unsigned j=0; j < newchan; j++) try {
      
    unsigned stchan = j * nscrunch;
    
    if (verbose)
      cerr << "Pulsar::Integration::fscrunch chan " << j << endl;
    
    double reference_frequency = weighted_frequency (stchan,stchan+nscrunch);

    if (must_dedisperse)
      dedisperse (stchan, stchan+nscrunch, dm, reference_frequency);

    if (must_defaraday)
      defaraday (stchan, stchan+nscrunch, rm, reference_frequency);
    
    for (unsigned ipol=0; ipol < get_npol(); ipol++)  {
      
      if (verbose)
	cerr << "Pulsar::Integration::fscrunch pol " << ipol << endl;
      
      *(profiles[ipol][j]) = *(profiles[ipol][stchan]);
      for (unsigned k=1; k<nscrunch; k++)
	*(profiles[ipol][j]) += *(profiles[ipol][stchan+k]);
      
    }

    set_centre_frequency (j, reference_frequency);

  }
  catch (Error& error) {
    throw error += "Integration::fscrunch";
  }

  if (verbose)
    cerr << "Pulsar::Integration::fscrunch resize" << endl;

  resize (0, newchan, 0);

  if (verbose) 
    cerr << "Pulsar::Integration::fscrunch finish" << endl;
} 
