#include "Archive.h"
#include "Integration.h"
#include "Profile.h"
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
  if (get_nsubint()<1 || get_npol()<1 || get_nchan()<1)
    throw Error (InvalidState, "Archive::total", "nsub=%d npol=%d nchan=%d",
		 get_nsubint(), get_npol(), get_nchan());
  
  Archive* copy = 0;

  try {
    copy = clone ();
    copy->fscrunch (0, false);
    copy->tscrunch ();
    copy->pscrunch ();
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Archive::total";
  }
  
  return copy;
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
  Archive* copy = 0;

  try {
    copy = total ();
    copy->get_Profile(0,0,0)->find_transitions (hi2lo, lo2hi, buf);
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Archive::find_transitions";
  }
  delete copy;
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
  Archive* copy = 0;

  try {
    copy = total ();
    copy->get_Profile(0,0,0)->find_peak_edges (rise, fall);
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Archive::find_peak_edges";
  }
  delete copy;
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
float Pulsar::Archive::find_min_phase () const
{
  Archive* copy = 0;
  float min_phase = -1.0;

  try {
    copy = total ();
    min_phase = copy->get_Profile(0,0,0)->find_min_phase ();
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Archive::find_min_phase";
  }
  delete copy;

  return min_phase;
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
  Archive* copy = 0;
  float max_phase = -1.0;

  try {
    copy = total ();
    max_phase = copy->get_Profile(0,0,0)->find_max_phase ();
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Archive::find_max_phase";
  }
  delete copy;

  return max_phase;
}
