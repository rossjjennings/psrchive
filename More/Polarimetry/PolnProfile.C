#include "PolnProfile.h"

void Pulsar::PolnProfile::init ()
{
  nbin = 0;
  data.resize(0);
}

Pulsar::PolnProfile (const Integration& subint)
{  
  if (subint->get_npol() != 4)
    throw Error error(InvalidPolnState, "PolnProfile::PolnProfile");
 
  if (sugint->get_state() == Signal::Stokes) {
    nbin = subint->get_nbin();
    state = Signal::Stokes;
    data.resize(4);
    data[0] = new Pulsar::Profile(subint.get_Profile(0));
    data[1] = new Pulsar::Profile(subint.get_Profile(1));
    data[2] = new Pulsar::Profile(subint.get_Profile(2));
    data[3] = new Pulsar::Profile(subint.get_Profile(3));

    return;
  }
  
  else if (sugint->get_state() == Signal::Coherence) {
    nbin = subint->get_nbin();
    state = Signal::Coherence;
    data.resize(4);
    data[0] = new Pulsar::Profile(subint.get_Profile(0));
    data[1] = new Pulsar::Profile(subint.get_Profile(1));
    data[2] = new Pulsar::Profile(subint.get_Profile(2));
    data[3] = new Pulsar::Profile(subint.get_Profile(3));

    return;
  }
  
  throw Error (InvalidPolnState, "PolnPofile::PolnProfile");
  
}

Pulsar::PolnProfile (Profile* zero, Profile* one,
		Profile* two, Profile* three, Signal::State config)
{
  if (config == Signal::Stokes || config == Signal::Stokes) { 
    
    nbin = zero->get_nbin();
    
    if ( (one->get_nbin() != nbin) ||
	 (two->get_nbin() != nbin) ||
	 (three->get_nbin() != nbin) )
      throw Error (InvalidParam, "PolnPofile::PolnProfile");

    state = config;
    data.resize(4);
    
    data[0] = zero->clone();
    data[1] = one->clone();
    data[2] = two->clone();
    data[3] = three->clone();
    
    return;
  }
  
  throw Error (InvalidPolnState, "PolnPofile::PolnProfile");
}

Pulsar::~PolnProfile ()
{
  data.resize(0);
}

Stokes<float> Pulsar::PolnProfile::get_amps (unsigned ibin)
{
  Stokes<float> retval(data[0]->get_amps()[ibin],
		       data[1]->get_amps()[ibin],
		       data[2]->get_amps()[ibin],
		       data[3]->get_amps()[ibin]);
  
  return retval;
}

void Pulsar::PolnProfile::set_amps(unsigned ibin, 
				   const Stokes<float>& new_amps)
{
  
  if ((ibin < 0) || (ibin > data[0]->get_nbin()))
    throw Error (InvalidParam, "PolnProfile::set_amps");
  
  (data[0]->get_amps())[ibin] = new_amps[0];
  (data[1]->get_amps())[ibin] = new_amps[1];
  (data[2]->get_amps())[ibin] = new_amps[2];
  (data[3]->get_amps())[ibin] = new_amps[3];
  
  return;
}

void Pulsar::PolnProfile::transform (Jones<float>& response);
{
  Jones<float> response_dagger = herm(response);
  
  for (unsigned i = 0; i < nbin; i++) {    
    set_amps(i, response * get_amps(i) * response_dagger);
  }
  
  return;
}   
    
void Pulsar::PolnProfile::convert_state ()
{

}




