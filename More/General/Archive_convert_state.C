#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  Simply calls Integration::convert_state for each Integration
*/
void Pulsar::Archive::convert_state (Signal::State state)
{
  if (get_nsubint() == 0)
    return;

  if (verbose)
    cerr << "Pulsar::Archive::convert_state current="
         << Signal::state_string (get_state()) << " request="
         << Signal::state_string (state) << endl;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> convert_state (state);

  set_npol ( get_Integration(0) -> get_npol() );
  set_state ( get_Integration(0) -> get_state() );
}
