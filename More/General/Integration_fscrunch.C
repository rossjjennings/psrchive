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
      " nadd=" << nscrunch << " dm=" << dm << endl;

  if (nscrunch == 1 || nchan == 1)
    // nothing to scrunch
    return;

  if (nscrunch == 0)
    nscrunch = nchan;

  if (nchan % nscrunch)
    throw Error (InvalidRange, "Pulsar::Integration::fscrunch",
		 "nchan=%d %% nscrunch=%d = %d",
		 nchan, nscrunch, nchan%nscrunch);

  int newchan = nchan/nscrunch;

  for (int j=0; j < newchan; j++) try {
      
    int stchan = j * nscrunch;
    
    if (verbose)
      cerr << "Pulsar::Integration::fscrunch - chan " << j << endl;
    
    if (dm != 0) {
      
      double fcentre = centrefreq;
      
      if (weighted_cfreq)
	fcentre = weighted_frequency (stchan, stchan+nscrunch);
      
      if (newchan == 1)
	centrefreq = fcentre;
      
      for (int ipol=0; ipol < npol; ipol++)
	for (int k=0; k<nscrunch; k++)
	  profiles[ipol][stchan+k]->dedisperse (dm,fcentre,pfold);
      
    }
    
    for (int ipol=0; ipol < npol; ipol++)  {
      
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

  resize (npol, newchan, nbin);
  
  if (verbose) 
    cerr << "Pulsar::Integration::fscrunch - finished successfully" << endl;
} 
