#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/Calculator.h"

//! Default constructor
Pulsar::BinaryPhaseOrder::BinaryPhaseOrder ()
  : IntegrationOrder ()
{
  IndexState =+ "Binary Phase (w.r.t. Ascending Node)";
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
  float minphase = 1.0;
  float maxphase = 0.0;
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    phases.push_back(get_binphs((arch->get_Integration(i)->get_epoch()).in_days(),
				arch->get_ephemeris(), 
				arch->get_Integration(i)->get_centre_frequency(),
				arch->get_telescope_code()));
    if (phases[i] > maxphase)
      maxphase = phases[i];
    
    if (phases[i] < minphase)
      minphase = phases[i];
  }
  
  Reference::To<Pulsar::Archive> copy = arch->clone();

  float PhaseGap = 0.01;
  
  arch->resize(int((maxphase - minphase)/PhaseGap));
  indices.resize(int((maxphase - minphase)/PhaseGap));
  
  Reference::To<Pulsar::Integration> integ = 0;
  
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    // Initialise with a blank Integration
    *(arch->get_Integration(i)) = *(arch->new_Integration());
    bool first = true;
    int tally = 0;
    for (unsigned j = 0; j < phases.size(); j++) {
      if ((phases[j] >= (minphase + (i*PhaseGap))) && 
	  (phases[j] < (minphase + ((i+1)*PhaseGap)))) {
	if (first) {
	  integ = arch->new_Integration(copy->get_Integration(j));
	  *(arch->get_Integration(i)) = *integ;
	  set_Index(i, Estimate<double>(phases[j], 0.0));
	  tally += 1;
	  first = false;
	}
	else {
	  *(arch->get_Integration(i)) += *(copy->get_Integration(j));
	  indices[i] += Estimate<double>(phases[j], 0.0);
	  tally += 1;
	}
      }
    }
    indices[i] /= tally;
  }
  
}

void Pulsar::BinaryPhaseOrder::append (Archive* thiz, const Archive* that)
{
  throw Error(FailedCall, "BinaryPhaseOrder::append",
	      "This method is not implemented");
}

void Pulsar::BinaryPhaseOrder::combine (Archive* arch, unsigned nscr)
{
  throw Error(FailedCall, "BinaryPhaseOrder::combine",
	      "This method is not implemented");
}
