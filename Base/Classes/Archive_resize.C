#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
  */

void Pulsar::Archive::resize (unsigned nsubint, unsigned npol,
			      unsigned nchan, unsigned nbin)
{
  unsigned cur_nsub = get_nsubint();
  unsigned cur_npol = get_npol();
  unsigned cur_nchan = get_nchan();
  unsigned cur_nbin = get_nbin();

  if (npol == 0)
    npol = cur_npol;
  if (nchan == 0)
    nchan = cur_nchan;
  if (nbin == 0)
    nbin = cur_nbin;
  
  if (verbose)
    cerr << "Archive::resize nsub=" << nsubint << " npol=" << npol
	 << " nchan=" << nchan << " nbin=" << nbin << endl 
	 << " old nsub=" << cur_nsub << " npol=" << cur_npol
	 << " nchan=" << cur_nchan << " nbin=" << cur_nbin << endl;


  if (verbose)
    cerr << "Pulsar::Archive::resize subints" << endl;
  IntegrationManager::resize (nsubint);

  indices.resize (nsubint);

  for (unsigned isub=0; isub<nsubint; isub++)
    get_Integration(isub) -> resize (npol, nchan, nbin);

  if (verbose)
    cerr << "Pulsar::Archive::resize calling book-keeping functions" << endl;

  set_npol (npol);
  set_nchan (nchan);
  set_nbin (nbin);

  if (verbose)
    cerr << "Pulsar::Archive::resize exit" << endl;
}

void Pulsar::Archive::resize_Integration (Integration* integration)
{
  integration->resize (get_npol(), get_nchan(), get_nbin());
}
