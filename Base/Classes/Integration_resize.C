#include "Integration.h"
#include "Profile.h"

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
  */
void Pulsar::Integration::resize (int new_npol, int new_nchan, int new_nbin)
{
  int cur_npol = get_npol();
  int cur_nchan = get_nchan();
  int cur_nbin = get_nbin();

  if (new_npol == 0)
    new_npol = cur_npol;
  if (new_nchan == 0)
    new_nchan = cur_nchan;
  if (new_nbin == 0)
    new_nbin = cur_nbin;

  if (verbose)
    cerr << "Integration::resize npol=" << new_npol
	 << " nchan=" << new_nchan << " nbin=" << new_nbin << endl 
	 << " old npol=" << cur_npol
	 << " nchan=" << cur_nchan << " nbin=" << cur_nbin << endl;

  int ipol=0, ichan=0;

  for (ipol=0; ipol < cur_npol; ipol++) {
    for (ichan=new_nchan; ichan < cur_nchan; ichan++) {
      if (profiles[ipol][ichan])
	delete profiles[ipol][ichan];  profiles[ipol][ichan]=0;
    }
    if (ipol < new_npol) {
      profiles[ipol].resize(new_nchan);
      for (ichan=cur_nchan; ichan < new_nchan; ichan++)
	profiles[ipol][ichan] = new_Profile();
    }
  }

  for (ipol=new_npol; ipol < cur_npol; ipol++)
    for (ichan=0; ichan < cur_nchan; ichan++) {
      if (profiles[ipol][ichan])
	delete profiles[ipol][ichan];  profiles[ipol][ichan]=0;
    }

  profiles.resize (new_npol);

  for (ipol=cur_npol; ipol < new_npol; ipol++) {
    profiles[ipol].resize(new_nchan);
    for (ichan=0; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] = new_Profile();
  }

  for (ipol=0; ipol < new_npol; ipol++)
    for (ichan=0; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] -> resize(new_nbin);

  set_npol (new_npol);
  set_nchan (new_nchan);
  set_nbin (new_nbin);

}
