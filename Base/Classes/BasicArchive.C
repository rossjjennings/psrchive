#include "BasicArchive.h"
#include "BasicIntegration.h"
#include "Error.h"

void Pulsar::BasicArchive::init ()
{
  feedtype = Signal::Linear;
  polstate = Signal::Intensity;
  obstype = Signal::Unknown;

  telcode = '\0';
    
  psrname = telid = frontend = backend = datatype = "None";
  
  nbin = nchan = npol = nsubint = 0;
  
  bandwidth = cenfreq = 0.0;
  
  calfreq = caldcyc = calphase = 0.0;
  
  facorr = pacorr = rm_ism = rm_iono = dedisp = true;
}

Pulsar::BasicArchive::~BasicArchive () 
{ 
  if (verbose)
    cerr << "Pulsar::BasicArchive::destructor" << endl;
}

const Pulsar::BasicArchive&
Pulsar::BasicArchive::operator = (const BasicArchive& copy)
{
  if (this == &copy)
    return *this;

  feedtype = copy.feedtype;

  // etc

  return *this;
}

/*!  
  By over-riding this funciton, inherited types may re-define the type
  of Integration to which the elements of the subints vector point.
*/
Pulsar::Integration*
Pulsar::BasicArchive::new_Integration (Integration* subint)
{
  Integration* integration;

  if (subint)
    integration = new BasicIntegration (*subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAlloc, "BasicArchive::new_Integration");
  
  return integration;
}

//! Get the number of sub-integrations in the archive
int Pulsar::BasicArchive::get_nsubint () const
{
  return nsubint;
}

//! Set the number of sub-integrations in the archive
void Pulsar::BasicArchive::set_nsubint (int num_sub) 
{
  nsubint = num_sub;
}

//! Get the number of frequency polns used
int Pulsar::BasicArchive::get_npol () const
{
  return npol;
}

//! Set the number of frequency polns used
void Pulsar::BasicArchive::set_npol (int numpol)
{
  npol = numpol;
}

//! Get the number of frequency channels used
int Pulsar::BasicArchive::get_nchan () const
{
  return nchan;
}

//! Set the number of frequency channels used
void Pulsar::BasicArchive::set_nchan (int numchan)
{
  nchan = numchan;
}

//! Get the number of pulsar phase bins used
int Pulsar::BasicArchive::get_nbin () const
{
  return nbin;
}

//! Set the number of pulsar phase bins used
void Pulsar::BasicArchive::set_nbin (int numbins)
{
  nbin = numbins;
}
