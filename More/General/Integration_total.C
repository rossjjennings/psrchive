#include "Pulsar/Integration.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::total
//
/*!
  This method is primarily designed for use by the Integration::find_*
  methods.  Integration::fscrunch is called with weighted_cfreq ==
  false so that the phase of the total profile will always relate to
  the centre frequency of the Integration.
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
    copy->fscrunch (0, false);
    copy->pscrunch ();
  }
  catch (Error& err) {
    throw err += "Integration::total";
  }

  return copy.release();
}

