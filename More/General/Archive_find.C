#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::total
//
/*!
  This method is primarily designed for use by the Archive::find_*
  methods.  Archive::fscrunch is called with weighted_cfreq ==
  false so that the phase of the total profile will always relate to
  the centre frequency of the Archive.
*/
Pulsar::Archive* Pulsar::Archive::total () const
{
  Reference::To<Archive> copy = clone ();
  
  copy->fscrunch (0, false);
  copy->tscrunch ();
  copy->pscrunch ();
    
  return copy.release();
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_transitions
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_transitions on the total intensity
  </UL>
*/
void Pulsar::Archive::find_transitions (int& hi2lo, int& lo2hi, int& buf)
  const
{
  Reference::To<Archive> copy = total ();
  copy->get_Profile(0,0,0)->find_transitions (hi2lo, lo2hi, buf);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_peak_edges
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_peak_edges on the total intensity
  </UL>
*/
void Pulsar::Archive::find_peak_edges (int& rise, int& fall) const
{
  Reference::To<Archive> copy = total ();
  copy->get_Profile(0,0,0)->find_peak_edges (rise, fall);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_min_phase
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_min_phase on the total intensity
  </UL>
*/
float Pulsar::Archive::find_min_phase (float dc) const
{
  Reference::To<Archive> copy = total ();
  return copy->get_Profile(0,0,0)->find_min_phase (dc);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_max_phase
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_max_phase on the total intensity
  </UL>
*/
float Pulsar::Archive::find_max_phase () const
{
  Reference::To<Archive> copy = total ();
  return copy->get_Profile(0,0,0)->find_max_phase ();
}
