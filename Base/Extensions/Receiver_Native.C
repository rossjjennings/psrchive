#include "Pulsar/Receiver_Native.h"

Pulsar::Receiver_Native::Receiver_Native ()
{
  basis = Signal::Linear;
  right_handed = true;
}

//! Copy the state from another
void Pulsar::Receiver_Native::copy (const State* state)
{
  set_basis (state->get_basis());
  set_orientation (state->get_orientation());
  set_right_handed (state->get_right_handed());
  set_reference_source_phase (state->get_reference_source_phase());
}
