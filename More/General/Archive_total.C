#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::total
//
/*!
  This method is primarily designed for use by the Archive::find_* methods.
*/
Pulsar::Archive* Pulsar::Archive::total () const
try {

  if (verbose)
    cerr << "Pulsar::Archive::total extracting an empty copy" << endl;

  vector<unsigned> empty;
  Reference::To<Archive> copy = extract (empty);

  if (verbose)
    cerr << "Pulsar::Archive::total adding " << get_nsubint()
         << "Integration::total copies" << endl;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    copy->manage( get_Integration(isub)->total() );

  copy->tscrunch ();

  return copy.release();
}
catch (Error& error) {
  throw error += "Pulsar::Archive::total";
}

