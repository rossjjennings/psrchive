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
  // Define a vector to hold the binary phases
  vector<float> phases;
  // Define a vector of flags to help avoid counting
  // subints twice when re-ordering
  vector<bool>  used;

  float minphs = 1.0;
  float maxphs = 0.0;
  
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    phases.push_back(get_binphs_peri((arch->get_Integration(i)->get_epoch()).in_days(),
				     arch->get_ephemeris(), 
				     arch->get_Integration(i)->get_centre_frequency(),
				     arch->get_telescope_code()));
    used.push_back(false);
    
    if (phases[i] > maxphs)
      maxphs = phases[i];
    
    if (phases[i] < minphs)
      minphs = phases[i];
  }

  // Interperate the newsub parameter as the number of integrations
  // required across a full phase wrap. This must be adjusted depending
  // on the phase coverage available in the archive
  
  float    phs_coverage = maxphs - minphs;
  unsigned mysub        = unsigned(phs_coverage * float(newsub));

  // This is equivalent to 1.0 / newsub given the above condition
  float PhaseGap = phs_coverage / float(mysub);

  // The "used" vector will ensure that no subints are counted
  // twice, but since they cannot be sub-divided, if you ask
  // for more subints in the resulting archive than effectively
  // exist in the original, the end result will include blank
  // integrations.

  Reference::To<Pulsar::Archive> copy = arch->clone();
  Reference::To<Pulsar::Integration> integ = 0;
  
  // Blank all the old data out
  arch->resize(0);
  // Resize for the new configuration
  arch->resize(mysub);
  indices.resize(mysub);
  
  for (unsigned i = 0; i < mysub; i++) {
    bool first = true;
    int tally = 0;
    for (unsigned j = 0; j < phases.size(); j++) {
      if ((phases[j] >= (minphs + (i*PhaseGap))) && 
	  (phases[j] < (minphs + ((i+1)*PhaseGap))) && !used[j]) {
	if (first) {
	  *(arch->get_Integration(i)) = 
	    *(arch->new_Integration(copy->get_Integration(j)));
	  set_Index(i, Estimate<double>(phases[j], 0.0));
	  used[j] = true;
	  tally += 1;
	  first = false;
	}
	else {
	  *(arch->get_Integration(i)) += *(copy->get_Integration(j));
	  indices[i] += Estimate<double>(phases[j], 0.0);
	  used[j] = true;
	  tally += 1;
	}
      }
    }
    if (tally > 0)
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
