#include "Archive.h"
#include "Integration.h"

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
  */

void Pulsar::Archive::resize (int nsubint, int npol, int nchan, int nbin)
{
  int cur_nsub = subints.size();
  int cur_npol = get_npol();
  int cur_nchan = get_nchan();
  int cur_nbin = get_nbin();

  if (nsubint == 0)
    nsubint = cur_nsub;
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

  int isub;
  for (isub=nsubint; isub<cur_nsub; isub++)
    delete subints[isub];

  subints.resize (nsubint);

  for (isub=cur_nsub; isub<nsubint; isub++)
    subints[isub] = new_Integration ();

  if (verbose)
    cerr << "Pulsar::Archive::resize subints" << endl;

  for (isub=0; isub<nsubint; isub++)
    subints[isub] -> resize (npol, nchan, nbin);

  if (verbose)
    cerr << "Pulsar::Archive::resize calling book-keeping functions" << endl;

  set_nsubint (nsubint);
  set_npol (npol);
  set_nchan (nchan);
  set_nbin (nbin);

  if (verbose)
    cerr << "Pulsar::Archive::resize exit" << endl;
}
