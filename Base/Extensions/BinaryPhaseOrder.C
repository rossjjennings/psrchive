#include "Pulsar/BinaryPhaseOrder.h"

//! Default constructor
Pulsar::BinaryPhaseOrder::BinaryPhaseOrder ()
  : IntegrationOrder ()
{
  IndexState = "BinaryPhase";
}

//! Destructor
Pulsar::BinaryPhaseOrder::~BinaryPhaseOrder ()
{

}

//! Copy constructor
Pulsar::BinaryPhaseOrder::BinaryPhaseOrder (const BinaryPhaseOrder& extension)
  : IntegrationOrder ()
{
  IndexState = extension.IndexState;
}

//! Operator =
const Pulsar::BinaryPhaseOrder&
Pulsar::BinaryPhaseOrder::operator= (const BinaryPhaseOrder& extension)
{
  IndexState = extension.IndexState;
  return *this;
}

//! Clone method
Pulsar::IntegrationOrder* Pulsar::BinaryPhaseOrder::clone () const
{
  return new BinaryPhaseOrder( *this ); 
}

// These are the methods that do the real work:

void Pulsar::BinaryPhaseOrder::organise (Archive* arch) 
{
  /* Not sure how to do this test...
     if (arch ! binary)
     throw Error(InvalidState, "Pulsar::Archive::convert_index_state",
     "The polyco has no binary phase");
  */
  
  vector<float> phases;
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    // phases.push_back(Binary_Phase(arch->get_Integration(i)->get_epoch()));
    // Not sure how to make the BinaryPhase function yet
  }
  
  Reference::To<Pulsar::Archive> copy = arch->clone();
  
  float PhaseGap = 0.1;
  
  arch->resize(int(1.0/PhaseGap));
  
  unsigned tally = 0;
  bool first = true;
  
  Reference::To<Pulsar::Integration> integ = 0;
  
  for (float i = 0.0; i < 1.0; i += PhaseGap) {
    first = true;
    for (unsigned j = 0; j < phases.size(); j++) {
      if (phases[j] >= i && phases[j] < i + PhaseGap) {
	if (first) {
	  integ = arch->new_Integration(copy->get_Integration(j));
	  *(arch->get_Integration(tally)) = *integ;
	  set_Index(tally, Estimate<double>(i + PhaseGap/2.0, PhaseGap/2.0));
	  first = false;
	}
	else {
	  *(arch->get_Integration(tally)) += *(copy->get_Integration(j));
	}
      }
    }
    tally += 1;
  }
}

void Pulsar::BinaryPhaseOrder::append (Archive* thiz, const Archive* that)
{
  
}

void Pulsar::BinaryPhaseOrder::combine (Archive* arch, unsigned nscr)
{

}
