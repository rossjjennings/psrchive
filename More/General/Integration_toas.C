#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/*! This method currently produces a toa for the first polarization state,
  be it the total intensity, XX, invariant interval, etc. */
void Pulsar::Integration::toas (vector<Tempo::toa>& toas,
				const Integration& std_subint,
				char nsite, bool discard_bad) const
{
  // empty the vector
  toas.resize (0);

  // get the standard profile
  const Profile* standard = std_subint.get_Profile (0, 0);

  // get the topocentric folding period
  double folding_period = get_folding_period();

  // get the mid time of the integration (rise time of bin 0 in each profile)
  MJD epoch = get_epoch ();

  // the time-of-arrival
  Tempo::toa toa;

  // auxilliary information
  char extra[20];

  for (unsigned ichan=0; ichan < get_nchan(); ++ichan) {

    const Profile* profile = get_Profile (0, ichan);

    if (discard_bad && profile->get_weight() == 0)
      continue;

    try {

      toa = profile->toa (standard, epoch, folding_period, nsite);

    }
    catch (Error& error)  {
      if (verbose)
	cerr << "Pulsar::Integration::toas error" << error << endl;
      continue;
    }

    sprintf (extra, "%d", ichan);
    toa.set_auxilliary_text (extra);

    toas.push_back (toa);

  }
}

