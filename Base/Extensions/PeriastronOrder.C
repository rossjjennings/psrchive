#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/Calculator.h"

//! Default constructor
Pulsar::PeriastronOrder::PeriastronOrder ()
  : IntegrationOrder ()
{
  IndexState = "Description Goes Here";
  Unit = "degrees";
}

//! Destructor
Pulsar::PeriastronOrder::~PeriastronOrder ()
{
}

//! Copy constructor
Pulsar::PeriastronOrder::PeriastronOrder (const PeriastronOrder& extension)
  : IntegrationOrder ()
{
  IndexState = extension.IndexState;
}

//! Operator =
const Pulsar::PeriastronOrder&
Pulsar::PeriastronOrder::operator= (const PeriastronOrder& extension)
{
  IndexState = extension.IndexState;
  return *this;
}

//! Clone method
Pulsar::IntegrationOrder* Pulsar::PeriastronOrder::clone () const
{
  return new PeriastronOrder( *this ); 
}

// These are the methods that do the real work:

void Pulsar::PeriastronOrder::organise (Archive* arch, unsigned newsub) 
{
  /* Not sure how to do this test...

     if (arch ! binary)
     throw Error(InvalidState, "Pulsar::Archive::convert_index_state",
     "This pulsar is not a binary system");
  */
  
  // Note that this next test doesn't account for a scenario where we
  // go from a progenitor archive with several wraps of binary
  // phase. The intrinsic resolution of the archive in phase space is
  // then less than the total number of subints and the result might
  // not look as expected. I'll fix this at some point... 
  //
  // AWH 30/12/2003

  if (newsub > arch->get_nsubint())
    throw Error(InvalidParam, "PeriastronOrder::organise",
		"Requested more subints than in the original archive");
  
  vector<float> phases;
  float minphs = 360.0;
  float maxphs = 0.0;
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    phases.push_back(get_binphs((arch->get_Integration(i)->get_epoch()).in_days(),
				arch->get_ephemeris(), 
				arch->get_Integration(i)->get_centre_frequency(),
				arch->get_telescope_code()));
    if (phases[i] > maxphs)
      maxphs = phases[i];
    
    if (phases[i] < minphs)
      minphs = phases[i];
  }
  
  Reference::To<Pulsar::Archive> copy = arch->clone();
  
  arch->resize(newsub);
  indices.resize(newsub);
  
  float phs_coverage = maxphs - minphs;
  float PhaseGap = phs_coverage / float(newsub);
  
  Reference::To<Pulsar::Integration> integ = 0;
  
  for (unsigned i = 0; i < newsub; i++) {
    // Initialise with a blank Integration
    *(arch->get_Integration(i)) = *(arch->new_Integration());
    bool first = true;
    int tally = 0;
    for (unsigned j = 0; j < phases.size(); j++) {
      if ((phases[j] >= (minphs + (i*PhaseGap))) && 
	  (phases[j] < (minphs + ((i+1)*PhaseGap)))) {
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

void Pulsar::PeriastronOrder::append (Archive* thiz, const Archive* that)
{
  throw Error(FailedCall, "BinaryPhaseOrder::append",
	      "This method is not implemented");
}

void Pulsar::PeriastronOrder::combine (Archive* arch, unsigned nscr)
{
  throw Error(FailedCall, "BinaryPhaseOrder::combine",
	      "This method is not implemented");
}
