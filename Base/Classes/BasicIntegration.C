#include "Pulsar/BasicIntegration.h"

/* not much to say really */
void Pulsar::BasicIntegration::init ()
{
  npol = nchan = nbin = 0;
  duration = centrefreq = bw = pfold = dm = 0.0;

  state = Signal::Intensity;
  type = Signal::Linear;
}

Pulsar::BasicIntegration::BasicIntegration (const Integration& subint,
					    int _npol, int _nchan)
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::BasicIntegration::BasicIntegration entered" << endl;
  
  init ();
  Integration::copy (subint);
}

//! Return the pointer to a new copy of self
Pulsar::Integration* 
Pulsar::BasicIntegration::clone (int npol, int nchan) const
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::BasicIntegration::clone entered" << endl;
  
  return new BasicIntegration (*this, npol, nchan);
}

