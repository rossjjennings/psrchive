#include "Pulsar/Integration.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::total
//
/*!
  This method is primarily designed for use by the Integration::find_*
  methods.  After calling fscrunch, the resulting Profile may have a 
  different centre frequency than that of the Integration.  Therefore,
  the Profile is dedispersed to match the phase referenced at the frequency
  returned by Integration::get_centre_frequency.
*/
Pulsar::Integration* Pulsar::Integration::total () const
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::total entered" << endl;

  if (get_npol()<1 || get_nchan()<1)
    throw Error (InvalidState, "Pulsar::Integration::total",
                 "npol=%d nchan=%d", get_npol(), get_nchan());

  int npol_keep = 1;
  if (get_state() == Signal::Coherence || get_state() == Signal::PPQQ)
    npol_keep = 2;

  Reference::To<Integration> copy;

  try {
    copy = clone (npol_keep);
    copy->pscrunch ();
    copy->fscrunch ();
    copy->dedisperse( get_centre_frequency() );
  }
  catch (Error& err) {
    throw err += "Integration::total";
  }

  return copy.release();
}

