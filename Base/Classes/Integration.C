#include <assert.h>

#include "Integration.h"
#include "Profile.h"
#include "Error.h"

bool Pulsar::Integration::verbose = false;

/* not much to say really */
void Pulsar::Integration::init ()
{
  npol = nchan = nbin = 0;
  duration = centrefreq = bw = pfold = dm = 0.0;

  state = Poln::invalid;
}

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  For instance, resize(0,0,0) will delete all profiles.
  */
void Pulsar::Integration::resize (int _npol, int _nchan, int _nbin)
{
  int ipol=0, ichan=0;

  for (ipol=0; ipol < npol; ipol++) {
    for (ichan=_nchan; ichan < nchan; ichan++) {
      if (profiles[ipol][ichan])
	delete profiles[ipol][ichan];  profiles[ipol][ichan]=0;
    }
    if (ipol < _npol) {
      profiles[ipol].resize(_nchan);
      for (ichan=nchan; ichan < _nchan; ichan++)
	profiles[ipol][ichan] = new_Profile();
    }
  }

  for (ipol=_npol; ipol < npol; ipol++)
    for (ichan=0; ichan < nchan; ichan++) {
      if (profiles[ipol][ichan])
	delete profiles[ipol][ichan];  profiles[ipol][ichan]=0;
    }

  profiles.resize (_npol);

  for (ipol=npol; ipol < _npol; ipol++) {
    profiles[ipol].resize(_nchan);
    for (ichan=0; ichan < _nchan; ichan++)
      profiles[ipol][ichan] = new_Profile();
  }

  npol = _npol;
  nchan = _nchan;

  for (ipol=0; ipol < npol; ipol++)
    for (ichan=0; ichan < nchan; ichan++)
      profiles[ipol][ichan] -> resize(_nbin);

  nbin = _nbin;
}

/*!
  By over-riding this method, inherited types may re-define the type
  of Profile to which the elements of the profiles vector point.
*/
Pulsar::Profile* Pulsar::Integration::new_Profile ()
{
  Profile* profile = new Profile;

  if (!profile)
    throw Error (BadAlloc, "Integration::new_Profile");

  return profile;
}

/* not much to say here, either */
Pulsar::Integration* Pulsar::Integration::clone (int _npol, int _nchan) const
{
  if (_npol == 0)
    _npol = npol;

  if (_nchan == 0)
    _nchan = nchan;

  if (_npol > npol)
    throw Error (InvalidRange, "Integration::clone",
		 "requested npol=%d.  have npol=%d", _npol, npol);

  if (_nchan > nchan)
    throw Error (InvalidRange, "Integration::clone",
		 "requested nchan=%d.  have nchan=%d", _nchan, nchan);

  Integration* ptr = new Integration;

  ptr->set_mid_time (mid_time);
  ptr->set_duration (duration);
  ptr->set_centre_frequency (centrefreq);
  ptr->set_bandwidth (bw);
  ptr->set_dispersion_measure (dm);
  ptr->set_folding_period (pfold);

  ptr->profiles.resize (_npol);

  for (int ipol=0; ipol<_npol; ipol++) {

    ptr->profiles[ipol].resize (_nchan);

    for (int ichan=0; ichan<_nchan; ichan++) {
      ptr->profiles[ipol][ichan] = profiles[ipol][ichan]->clone();
      // sanity check
      assert (ptr->profiles[ipol][ichan] != 0);
    }

  }

  return ptr;
}

Pulsar::Profile* Pulsar::Integration::get_Profile (int ipol, int ichan)
{
  if (ipol < 0 || ipol>=npol)
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ipol=%d npol=%d", ipol, npol);

  if (ichan < 0 || ichan>=nchan)
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ichan=%d nchan=%d", ichan, nchan);

  return profiles[ipol][ichan];
}

vector<Pulsar::Profile*>& Pulsar::Integration::operator[] (Poln::Measure poln)
{
  switch (state) 
    {

    case Poln::Stokes:
      switch (poln)
	{
	case Poln::Si:
	  return profiles[0];
	case Poln::Sq:
	  return profiles[1];
	case Poln::Su:
	  return profiles[2];
	case Poln::Sv:
	  return profiles[3];
	default:
	  break;
	}
      break;

    case Poln::Coherence:
      switch (poln) 	{
	case Poln::RR:
	case Poln::XX:
	  return profiles[0];
	case Poln::LL:
	case Poln::YY:
	  return profiles[1];
	case Poln::ReRL:
	case Poln::ReXY:
	  return profiles[2];
	case Poln::ImRL:
	case Poln::ImXY:
	  return profiles[3];
	default:
	  break;
	}
      break;
  
    case Poln::PPQQ:
      switch (poln) 
	{
	case Poln::RR:
	case Poln::XX:
	  return profiles[0];
	case Poln::LL:
	case Poln::YY:
	  return profiles[1];
	default:
	  break;
	}
      break;
      
    case Poln::Intensity:
      if (poln == Poln::Si) 
	return profiles[0];
      break;

    case Poln::Invariant:
      if (poln == Poln::SInv || poln == Poln::DetRho) 
	return profiles[0];
      break;

    default:
      break;
    }

  throw Error (InvalidPolnState, "Integration::operator[]");
}

void Pulsar::Integration::fold (int nfold)
{
  if (npol<1 || nchan<1)
    return;

  try {
    for (int ipol=0; ipol<npol; ipol++)
      for (int ichan=0; ichan<nchan; ichan++)
	profiles[ipol][ichan] -> fold (nfold);

    nbin = profiles[0][0] -> get_nbin();
  }
  catch (Error& error) {
    throw error += "Integration::fold";
  }
}

void Pulsar::Integration::bscrunch (int nscrunch)
{
  if (npol<1 || nchan<1)
    return;

  try {
    for (int ipol=0; ipol<npol; ipol++)
      for (int ichan=0; ichan<nchan; ichan++)
	profiles[ipol][ichan] -> bscrunch (nscrunch);

    nbin = profiles[0][0] -> get_nbin();
  }
  catch (Error& error) {
    throw error += "Integration::bscrunch";
  }
}
    
void Pulsar::Integration::pscrunch()
{
  if (state == Poln::Coherence || state == Poln::PPQQ) {

    if (npol != 2)
      throw Error (InvalidState, "Integration::pscrunch", "npol != 2");

    for (int ichan=0; ichan < nchan; ichan++)
      *(profiles[0][ichan]) += *(profiles[1][ichan]);

    state = Poln::Intensity;
  }

  else if (state == Poln::Stokes)
    state = Poln::Intensity;

  resize (1, nchan, nbin);
} 

void Pulsar::Integration::rotate (double time)
{
  if (pfold <= 0.0)
    throw Error (InvalidParam, "Integration::rotate",
		 "folding period=%lf", pfold);

  try {
    for (int ipol=0; ipol<npol; ipol++)
      for (int ichan=0; ichan<nchan; ichan++)
	profiles[ipol][ichan] -> rotate (time/pfold);

    set_mid_time (get_mid_time() + time);
  }
  catch (Error& error) {
    throw error += "Integration::rotate";
  }
}
