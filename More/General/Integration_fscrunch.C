#include "Integration.h"
#include "Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::fscrunch
//
/*!
  \param n_add number of neighbouring bands to integrate.  If n_add == 0,
  then integrate all bands
  \param weighted_cfreq if true, calculate a new centre frequency for result
  \
 */
void Pulsar::Integration::fscrunch (int n_add, bool weighted_cfreq)
{
  if (verbose)
    cerr << "Pulsar::Integration::fscrunch"
      " nadd=" << n_add << " dm=" << dm << endl;

  if (nband <= 1 || n_add <= 1)  {
    if (verbose)
      cerr << "Pulsar::Integration::fscrunch invalid nband or n_add" << endl;
    return;
  }

  if (n_add == 0)
    n_add = nband;

  if (nband % n_add)
    throw Error (InvalidRange, "Pulsar::Integration::fscrunch",
		 "nband=%d %% n_add=%d = %d", nband, n_add, nband%n_add);

  int newband = nband/n_add;

  try {

    for (int j=0; j < newband; j++) {
      
      int stband = j * n_add;
      
      if (verbose)
	cerr << "Pulsar::Integration::fscrunch - band " << j << endl;
      
      if (dm != 0) {
	
	double weight = 1.0;
	double fcentre = centrefreq;

	if (weighted_cfreq)
	  fcentre = weighted_frequency (&weight, stband, stband+n_add);
	
	if (newband == 1)
	  centrefreq = fcentre;
	
	for (int ipol=0; ipol < npol; ipol++)
	  for (int k=0; k<n_add; k++)
	    profiles[ipol][stband+k]->dedisperse (dm,fcentre,pfold);
	
      }
      
      for (int ipol=0; ipol < npol; ipol++)  {
	
	if (verbose)
	  cerr << "Pulsar::Integration::fscrunch pol " << ipol << endl;
	
	*(profiles[ipol][j]) = *(profiles[ipol][stband]);
	for (int k=1; k<n_add; k++)
	  *(profiles[ipol][j]) += *(profiles[ipol][stband+k]);
	
      }
    }

    resize (npol, newband, nbin);
  
  }
  catch (Error& error) {
    throw error += "Integration::fscrunch";
  }

  if (verbose) 
    cerr << "Pulsar::Integration::fscrunch - finished successfully" << endl;
} 
