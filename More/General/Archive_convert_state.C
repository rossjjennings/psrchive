#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Estimate.h"

/*!
  Simply calls Integration::convert_state for each Integration
*/
void Pulsar::Archive::convert_state (Signal::State state)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> convert_state (state);

  set_npol ( get_Integration(0) -> get_npol() );
  set_state ( get_Integration(0) -> get_state() );
}

/*!
  Works out what transformation is required and re-orders the
  Integrations as necessary, updating the indices vector
*/
void Pulsar::Archive::convert_index_state (IndexState state)
{
  if (get_nsubint() == 0)
    return;
  
  if (index_state != TimeOrder)
    throw Error (InvalidState, "Pulsar::Archive::convert_index_state",
		 "Cannot revert back to TimeOrder");
  
  switch (state) {

  case TimeOrder: {
    // Nothing to do
    return;
  }
  break;

  case BinaryPhase: {
    
    /* Not sure how to do this test...
       if (!binary)
         throw Error(InvalidState, "Pulsar::Archive::convert_index_state",
                     "The polyco has no binary phase");
    */    
    
    vector<float> phases;
    for (unsigned i = 0; i < get_nsubint(); i++) {
      //phases.push_back(Binary_Phase(get_Integration(i)->get_epoch()));
      // Not sure how to make the BinaryPhase function yet
    }
    
    Reference::To<Pulsar::Archive> copy = this->clone();

    float PhaseGap = 0.1;
    
    resize(int(1.0/PhaseGap));
    
    unsigned tally = 0;
    bool first = true;
    
    Reference::To<Pulsar::Integration> integ = 0;
    
    for (float i = 0.0; i < 1.0; i += PhaseGap) {
      first = true;
      for (unsigned j = 0; j < phases.size(); j++) {
	if (phases[j] >= i && phases[j] < i + PhaseGap) {
	  if (first) {
	    integ = new_Integration(copy->get_Integration(j));
	    *(get_Integration(tally)) = *integ;
	    set_Index(tally, Estimate<double>(i + PhaseGap/2.0, PhaseGap/2.0));
	    first = false;
	  }
	  else {
	    *(get_Integration(tally)) += *(copy->get_Integration(j));
	  }
	}
      }
      tally += 1;
    }

    index_state = BinaryPhase;
  }
  break;
  
  case LoAscNode: {
    // Operations go here
    index_state = LoAscNode;
  }
  break;

  }
}


