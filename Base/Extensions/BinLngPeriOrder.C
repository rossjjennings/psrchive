#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/Calculator.h"

//! Default constructor
Pulsar::BinLngPeriOrder::BinLngPeriOrder ()
  : IntegrationOrder ()
{
  IndexState = "Binary Longitude (w.r.t. Periastron)";
  Unit = "degrees";
}

//! Destructor
Pulsar::BinLngPeriOrder::~BinLngPeriOrder ()
{
}

//! Copy constructor
Pulsar::BinLngPeriOrder::BinLngPeriOrder (const BinLngPeriOrder& extension)
  : IntegrationOrder ()
{
  IndexState = extension.IndexState;
}

//! Operator =
const Pulsar::BinLngPeriOrder&
Pulsar::BinLngPeriOrder::operator= (const BinLngPeriOrder& extension)
{
  IndexState = extension.IndexState;
  return *this;
}

//! Clone method
Pulsar::IntegrationOrder* Pulsar::BinLngPeriOrder::clone () const
{
  return new BinLngPeriOrder( *this ); 
}

// These are the methods that do the real work:

void Pulsar::BinLngPeriOrder::organise (Archive* arch, unsigned newsub) 
{
  // Define a vector to hold the binary lngs
  vector<float> lngs;
  // Define a vector of flags to help avoid counting
  // subints twice when re-ordering
  vector<bool>  used;

  float minlng = 360.0;
  float maxlng = 0.0;
  
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    lngs.push_back(get_binlng_peri((arch->get_Integration(i)->get_epoch()).in_days(),
				   arch->get_ephemeris(), 
				   arch->get_Integration(i)->get_centre_frequency(),
				   arch->get_telescope_code()));
    used.push_back(false);
    
    if (lngs[i] > maxlng)
      maxlng = lngs[i];
    
    if (lngs[i] < minlng)
      minlng = lngs[i];
  }

  // Interperate the newsub parameter as the number of integrations
  // required across a full phase wrap. This must be adjusted depending
  // on the phase coverage available in the archive
  
  float    phs_coverage = maxlng - minlng;
  unsigned mysub        = unsigned(phs_coverage * float(newsub));

  // This is equivalent to 360.0 / newsub given the above condition
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
    for (unsigned j = 0; j < lngs.size(); j++) {
      if ((lngs[j] >= (minlng + (i*PhaseGap))) && 
	  (lngs[j] < (minlng + ((i+1)*PhaseGap))) && !used[j]) {
	if (first) {
	  *(arch->get_Integration(i)) = 
	    *(arch->new_Integration(copy->get_Integration(j)));
	  set_Index(i, Estimate<double>(lngs[j], 0.0));
	  used[j] = true;
	  tally += 1;
	  first = false;
	}
	else {
	  *(arch->get_Integration(i)) += *(copy->get_Integration(j));
	  indices[i] += Estimate<double>(lngs[j], 0.0);
	  used[j] = true;
	  tally += 1;
	}
      }
    }
    if (tally > 0)
      indices[i] /= tally;
  }

}

void Pulsar::BinLngPeriOrder::append (Archive* thiz, const Archive* that)
{
  throw Error(FailedCall, "BinLngPeri::append",
	      "This method is not implemented");
}

void Pulsar::BinLngPeriOrder::combine (Archive* arch, unsigned nscr)
{
  throw Error(FailedCall, "BinLngPeriOrder::combine",
	      "This method is not implemented");
}
