#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/IntegrationOrder.h"

/*!
  \param standard
  \retval toas
*/
void Pulsar::Archive::toas (vector<Tempo::toa>& toas,
			    const Archive* standard,
			    string arguments,
			    Tempo::toa::Format fmt) const
{
  if (get<IntegrationOrder>()) {
    throw Error (InvalidState, "Pulsar::Archive::toas",
		 "Archive not in time order");
  }
  
  toas.resize (0);

  string reason;
  if (!standard_match (standard, reason))
    cerr << "Pulsar::Archive::toas WARNING " << reason << endl;

  double cf1 = get_centre_frequency();
  double cf2 = standard->get_centre_frequency();
  double dfreq = fabs (cf2 - cf1);

  if (dfreq > 0.2 * cf1)
    cerr << "Pulsar::Archive::toas WARNING standard centre frequency = " << cf2
	 << " != " << cf1 << endl;

  const Integration* std = standard->get_Integration (0);

  char nsite = get_telescope_code();

  for (unsigned isub=0; isub<get_nsubint(); isub++) {
    
    vector<Tempo::toa> toaset;
    get_Integration(isub)->toas (toaset, *std, nsite, arguments, fmt);
    
    if (fmt == Tempo::toa::Parkes || fmt == Tempo::toa::Psrclock) {
      // some extra information to place in each toa
      char extra[20];
      sprintf (extra, " %d ", isub);
      for (unsigned itoa=0; itoa < toaset.size(); itoa++) {
	string aux = get_filename() + extra + toaset[itoa].get_auxilliary_text();
	toaset[itoa].set_auxilliary_text (aux);
	toas.push_back (toaset[itoa]);
      }
    }
    else {
      for (unsigned itoa=0; itoa < toaset.size(); itoa++) {
        toas.push_back (toaset[itoa]);
      }
    }

  }
}
