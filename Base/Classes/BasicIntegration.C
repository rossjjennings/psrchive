#include "BasicIntegration.h"

/* not much to say really */
void Pulsar::BasicIntegration::init ()
{
  npol = nchan = nbin = 0;
  duration = centrefreq = bw = pfold = dm = 0.0;

  state = Poln::invalid;
  type = Feed::invalid;
}

Pulsar::BasicIntegration::BasicIntegration (const Integration& subint,
					    int _npol, int _nchan)
  : Integration (subint, _npol, _nchan)
{
  set_mid_time ( subint.get_mid_time());
  set_duration ( subint.get_duration());
  set_centre_frequency ( subint.get_centre_frequency() );
  set_bandwidth ( subint.get_bandwidth() );
  set_dispersion_measure ( subint.get_dispersion_measure() );
  set_folding_period ( subint.get_folding_period() );
}
