#include "Integration.h"
#include "Profile.h"

void Pulsar::Integration::resize (int _npol, int _nband, int _nbin)
{
  int ipol=0, iband=0;

  for (ipol=0; ipol < npol; ipol++) {
    for (iband=_nband; iband < nband; iband++) {
      if (profiles[ipol][iband])
	delete profiles[ipol][iband];  profiles[ipol][iband]=0;
    }
    if (ipol < _npol) {
      profiles[ipol].resize(_nband);
      for (iband=nband; iband < _nband; iband++)
	profiles[ipol][iband] = new_Profile();
    }
  }

  for (ipol=_npol; ipol < npol; ipol++)
    for (iband=0; iband < nband; iband++) {
      if (profiles[ipol][iband])
	delete profiles[ipol][iband];  profiles[ipol][iband]=0;
    }

  profiles.resize (_npol);

  for (ipol=npol; ipol < _npol; ipol++) {
    profiles[ipol].resize(_nband);
    for (iband=0; iband < _nband; iband++)
      profiles[ipol][iband] = new_Profile();
  }

  npol = _npol;
  nband = _nband;

  for (ipol=0; ipol < npol; ipol++)
    for (iband=0; iband < nband; iband++)
      profiles[ipol][iband] -> resize(_nbin);

  nbin = _nbin;
}

/*!  
  All new profiles will be created through this function.  By
  over-riding this funciton, inherited types may re-define the type of
  profile contained in the profiles vector.
*/
Pulsar::Profile* Pulsar::Integration::new_Profile ()
{
  return new Profile;
}

