#include "Archive.h"
#include "Integration.h"
#include "Error.h"

//! Perform the transformation on each polarimetric profile
void Pulsar::Archive::transform (const Jones<float>& jones)
{
  try {

    unsigned nsub = get_nsubint ();

    for (unsigned isub=0; isub < nsub; isub++)
      get_Integration (isub) -> transform (jones);

    if (nsub > 0)
      set_state (get_Integration(0)->get_state());

  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::transform (Jones<float>)";
  }
}

//! Perform frequency response on each polarimetric profile
void Pulsar::Archive::transform (const vector<Jones<float> >& jones)
{
  try {

    unsigned nsub = get_nsubint ();

    for (unsigned isub=0; isub < nsub; isub++)
      get_Integration (isub) -> transform (jones);
    
    if (nsub > 0)
      set_state (get_Integration(0)->get_state());
    
  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::transform (vector<Jones<float>>)";
  }
}

//! Perform the time and frequency response on each polarimetric profile
void Pulsar::Archive::transform (const vector<vector<Jones<float> > >& jones)
{
  try {

    unsigned nsub = get_nsubint ();

    for (unsigned isub=0; isub < nsub; isub++)
      get_Integration (isub) -> transform (jones[isub]);
    
    if (nsub > 0)
      set_state (get_Integration(0)->get_state());

  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::transform (vector<vector<Jones<float>>>)";
  }
}


