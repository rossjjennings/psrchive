#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/*!
  \param standard
  \retval toas
*/
void Pulsar::Archive::toas (vector<Tempo::toa>& toas,
			    const Archive* standard) const
{
  string reason;
  if (!standard_match (standard, reason))
    cerr << "Pulsar::Archive::toas WARNING " << reason << endl;

  const Integration* std = standard->get_Integration (0);

  char nsite = get_telescope_code();

  for (unsigned isub=0; isub<get_nsubint(); isub++) {

    // some extra information to place in each toa
    char extra[20];
    sprintf (extra, " %d ", isub);

    vector<Tempo::toa> toaset;
    get_Integration(isub)->toas (toaset, *std, nsite);

    for (unsigned itoa=0; itoa < toaset.size(); itoa++) {
      string aux = get_filename() + extra + toaset[itoa].get_auxilliary_text();
      toaset[itoa].set_auxilliary_text (aux);
      toas.push_back (toaset[itoa]);
    }

  }
}

