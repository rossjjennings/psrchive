#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::fscrunch
//
/*!
  \param nscrunch number of neighbouring chans to integrate.  If nscrunch == 0,
  then integrate all chans
  \param weighted_cfreq if true, calculate a new centre frequency for result
  \
 */
void Pulsar::Integration::fscrunch (unsigned nscrunch, bool weighted_cfreq)
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
  double pfold = get_folding_period();

  for (unsigned j=0; j < newchan; j++) try {
      
    unsigned stchan = j * nscrunch;
    
    if (verbose)
      cerr << "Pulsar::Integration::fscrunch chan " << j << endl;
    
    if (dm != 0) {
      
      double fcentre = get_centre_frequency();
      
      if (weighted_cfreq)
	fcentre = weighted_frequency (stchan, stchan+nscrunch);

      // WvS - the centre_frequency of the Integration should be changed only
      // by the Archive class.  This is because the Archive class expects to
      // treat each Integration equally (e.g. in the calculation of relative
      // phases).  It is then up to the Integration class to translate 
      // frequency-dependent calculations using the centre frequency of each
      // Profile.  If sparse Archive support is enabled in the future, each
      // Integration may require a unique centre frequency.
      //
      // if (newchan == 1)
      //   set_centre_frequency (fcentre);
      
      for (unsigned ipol=0; ipol < get_npol(); ipol++)
	for (unsigned k=0; k<nscrunch; k++)
	  profiles[ipol][stchan+k]->dedisperse (dm,fcentre,pfold);
      
    }
    
    for (unsigned ipol=0; ipol < get_npol(); ipol++)  {
      
      if (verbose)
	cerr << "Pulsar::Integration::fscrunch pol " << ipol << endl;
      
      *(profiles[ipol][j]) = *(profiles[ipol][stchan]);
      for (unsigned k=1; k<nscrunch; k++)
	*(profiles[ipol][j]) += *(profiles[ipol][stchan+k]);
      
    }
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
