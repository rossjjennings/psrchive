#include "Integration.h"
#include "Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::fscrunch
//
/*!
  \param n_add number of neighbouring chans to integrate.  If n_add == 0,
  then integrate all chans
  \param weighted_cfreq if true, calculate a new centre frequency for result
  \
 */
void Pulsar::Integration::fscrunch (int n_add, bool weighted_cfreq)
{
  if (verbose)
    cerr << "Pulsar::Integration::fscrunch"
      " nadd=" << n_add << " dm=" << dm << endl;

  if (nchan <= 1 || n_add <= 1)  {
    if (verbose)
      cerr << "Pulsar::Integration::fscrunch invalid nchan or n_add" << endl;
    return;
  }

  if (n_add == 0)
    n_add = nchan;

  if (nchan % n_add)
    throw Error (InvalidRange, "Pulsar::Integration::fscrunch",
		 "nchan=%d %% n_add=%d = %d", nchan, n_add, nchan%n_add);

  int newchan = nchan/n_add;

  try {

    for (int j=0; j < newchan; j++) {
      
      int stchan = j * n_add;
      
      if (verbose)
	cerr << "Pulsar::Integration::fscrunch - chan " << j << endl;
      
      if (dm != 0) {
	
	double weight = 1.0;
	double fcentre = centrefreq;

	if (weighted_cfreq)
	  fcentre = weighted_frequency (&weight, stchan, stchan+n_add);
	
	if (newchan == 1)
	  centrefreq = fcentre;
	
	for (int ipol=0; ipol < npol; ipol++)
	  for (int k=0; k<n_add; k++)
	    profiles[ipol][stchan+k]->dedisperse (dm,fcentre,pfold);
	
      }
      
      for (int ipol=0; ipol < npol; ipol++)  {
	
	if (verbose)
	  cerr << "Pulsar::Integration::fscrunch pol " << ipol << endl;
	
	*(profiles[ipol][j]) = *(profiles[ipol][stchan]);
	for (int k=1; k<n_add; k++)
	  *(profiles[ipol][j]) += *(profiles[ipol][stchan+k]);
	
      }
    }

    resize (npol, newchan, nbin);
  
  }
  catch (Error& error) {
    throw error += "Integration::fscrunch";
  }

  if (verbose) 
    cerr << "Pulsar::Integration::fscrunch - finished successfully" << endl;
} 
