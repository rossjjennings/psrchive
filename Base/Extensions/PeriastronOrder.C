#include "Pulsar/PeriastronOrder.h"

//! Default constructor
Pulsar::PeriastronOrder::PeriastronOrder ()
  : IntegrationOrder ()
{
  IndexState = "Periastron";
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

void Pulsar::PeriastronOrder::organise (Archive* arch) 
{
  /* Not sure how to do this test...

     if (arch ! binary)
     throw Error(InvalidState, "Pulsar::Archive::convert_index_state",
     "This pulsar is not a binary system");
  */
  
  vector<float> vals;
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    // vals.push_back(Periastron(arch->get_Integration(i)->get_epoch()));
  }
  
  Reference::To<Pulsar::Archive> copy = arch->clone();
  
  // Gap is the resolution with which you want to construct the
  // new indexing scheme. In this case, you will get one subint
  // per ten degrees of phase. You may wish to change this.
  // This value should represent the maximum useful resolution,
  // we can use the PeriastronOrder::combine method below to sum the
  // Integrations and decrease the resolution.

  float Gap = 10.0;
  
  arch->resize(int(360.0/Gap));
  
  unsigned tally = 0;
  bool first = true;
  
  Reference::To<Pulsar::Integration> integ = 0;
  
  // I've assumed that Long. wrt Asc. Node is specified
  // between +/- 180 degrees. If that is incorrect, you
  // can change the ranges in the for loop below:

  for (float i = -180.0; i < 180.0; i += Gap) {
    first = true;
    for (unsigned j = 0; j < vals.size(); j++) {
      if (vals[j] >= i && vals[j] < i + Gap) {
	if (first) {
	  integ = arch->new_Integration(copy->get_Integration(j));
	  *(arch->get_Integration(tally)) = *integ;
	  set_Index(tally, Estimate<double>(i + Gap/2.0, Gap/2.0));
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

void Pulsar::PeriastronOrder::append (Archive* thiz, const Archive* that)
{
  // This method will describe how to sum data from another
  // archive that is also indexed by Periastron.
}

void Pulsar::PeriastronOrder::combine (Archive* arch, unsigned nscr)
{
  // This method will describe how to scrunch down the index
  // resolution.
}
