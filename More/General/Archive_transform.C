#include "Archive.h"
#include "Integration.h"

//! Perform the transformation on each polarimetric profile
void Pulsar::Archive::transform (const Jones<float>& jones)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration (isub) -> transform (jones);

  if (get_nsubint() > 0)
    set_state (get_Integration(0)->get_state());
}

//! Perform frequency response on each polarimetric profile
void Pulsar::Archive::transform (const vector<Jones<float> >& jones)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration (isub) -> transform (jones);

  if (get_nsubint() > 0)
    set_state (get_Integration(0)->get_state());
}

//! Perform the time and frequency response on each polarimetric profile
void Pulsar::Archive::transform (const vector<vector<Jones<float> > >& jones)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration (isub) -> transform (jones[isub]);

  if (get_nsubint() > 0)
    set_state (get_Integration(0)->get_state());
}
