#include "Integration.h"
#include "Profile.h"
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
void Pulsar::Integration::fscrunch (int nscrunch, bool weighted_cfreq)
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

  int newchan = get_nchan()/nscrunch;

  double dm = get_dispersion_measure();
  double pfold = get_folding_period();

  for (int j=0; j < newchan; j++) try {
      
    int stchan = j * nscrunch;
    
    if (verbose)
      cerr << "Pulsar::Integration::fscrunch chan " << j << endl;
    
    if (dm != 0) {
      
      double fcentre = get_centre_frequency();
      
      if (weighted_cfreq)
	fcentre = weighted_frequency (stchan, stchan+nscrunch);
      
      if (newchan == 1)
	set_centre_frequency (fcentre);
      
      for (int ipol=0; ipol < get_npol(); ipol++)
	for (int k=0; k<nscrunch; k++)
	  profiles[ipol][stchan+k]->dedisperse (dm,fcentre,pfold);
      
    }
    
    for (int ipol=0; ipol < get_npol(); ipol++)  {
      
      if (verbose)
	cerr << "Pulsar::Integration::fscrunch pol " << ipol << endl;
      
      *(profiles[ipol][j]) = *(profiles[ipol][stchan]);
      for (int k=1; k<nscrunch; k++)
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
