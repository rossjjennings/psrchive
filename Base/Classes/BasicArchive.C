#include "BasicArchive.h"

void Pulsar::BasicArchive::init ()
{
  feedtype = Feed::invalid;
  polstate = Poln::invalid;
    
  telcode = '\0';
    
  psrname = telid = frontend = backend = obstype = datatype = "None";
  
  nbin = nchan = npol = nsubint = 0;
  
  chanbw = bandwidth = cenfreq = 0.0;
  
  calfreq = caldcyc = calphase = 0.0;
  
  facorr = pacorr = rm_ism = rm_iono = dedisp = true;
};
  
const Pulsar::BasicArchive&
Pulsar::BasicArchive::operator = (const BasicArchive& copy)
{
  if (this == &copy)
    return *this;

  feedtype = copy.feedtype;

  // etc

  return *this;
}
