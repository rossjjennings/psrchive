#include <assert.h>

#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

bool Pulsar::Integration::verbose = false;


Pulsar::Integration::Integration ()
{
  // nothing to initialize
}

Pulsar::Integration::Integration (const Integration& subint)
{
  throw Error (Undefined, "Integration copy constructor",
	       "sub-classes must define copy constructor");
}

Pulsar::Integration& Pulsar::Integration::operator= (const Integration& subint)
{
  copy (subint);
  return *this;
}

Pulsar::Integration::~Integration ()
{
  if (verbose)
    cerr << "Pulsar::Integration destructor" << endl;
}


/*!
  By over-riding this method, inherited types may re-define the type
  of Profile to which the elements of the profiles vector point.
*/
Pulsar::Profile* Pulsar::Integration::new_Profile ()
{
  Profile* profile = new Profile;

  if (!profile)
    throw Error (BadAllocation, "Integration::new_Profile");

  return profile;
}

/*!
  Copy the commonn information from another Integration
*/
void Pulsar::Integration::copy (const Integration& subint,
				int _npol, int _nchan)
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::copy entered" << endl;

  if (this == &subint)
    return;

  if (_npol < 0)
    _npol = subint.get_npol();

  if (_nchan < 0)
    _nchan = subint.get_nchan();

  if (unsigned(_npol) > subint.get_npol())
    throw Error (InvalidRange, "Integration copy constructor",
		 "requested npol=%d.  npol=%d", _npol, subint.get_npol());
  
  if (unsigned(_nchan) > subint.get_nchan())
    throw Error (InvalidRange, "Integration copy constructor",
		 "requested nchan=%d.  nchan=%d", _nchan, subint.get_nchan());

  resize (_npol, _nchan, subint.get_nbin());

  for (int ipol=0; ipol<_npol; ipol++)
    for (int ichan=0; ichan<_nchan; ichan++)
      *(profiles[ipol][ichan]) = *(subint.profiles[ipol][ichan]);

  set_mid_time ( subint.get_mid_time());
  set_duration ( subint.get_duration());
  set_centre_frequency ( subint.get_centre_frequency() );
  set_bandwidth ( subint.get_bandwidth() );
  set_dispersion_measure ( subint.get_dispersion_measure() );
  set_folding_period ( subint.get_folding_period() );
  set_basis ( subint.get_basis() );
  set_state ( subint.get_state() );
}

Pulsar::Profile*
Pulsar::Integration::get_Profile (unsigned ipol, unsigned ichan)
{
  if (ipol>=get_npol())
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ipol=%d npol=%d", ipol, get_npol());

  if (ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  return profiles[ipol][ichan];
}

const Pulsar::Profile*
Pulsar::Integration::get_Profile (unsigned ipol, unsigned ichan) const
{
  if (ipol>=get_npol())
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ipol=%d npol=%d", ipol, get_npol());

  if (ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  return profiles[ipol][ichan];
}

void Pulsar::Integration::get_amps (float* data,
				    unsigned jpol,
				    unsigned jchan,
				    unsigned jbin) const
{
  unsigned npol = get_npol();
  unsigned nchan = get_nchan();

  if (verbose)
    cerr << "int.npol=" << npol << " int.nchan=" << nchan << endl;

  for (unsigned ipol=0; ipol<npol; ipol++) {
    float* chandat = data + ipol * jpol;
    for (unsigned ichan=0; ichan<nchan; ichan++)
      profiles[ipol][ichan] -> get_amps (chandat + ichan * jchan, jbin);
  }
}

//! Get the frequency of the given channel
/*!
  \param ichan the index of the channel to get
  \return the frequency of the given channel in MHz
*/
double Pulsar::Integration::get_frequency (unsigned ichan) const
{
  if (ichan < 0 || ichan>=get_nchan() || get_npol() < 1)
    return 0;

  return profiles[0][ichan]->get_centre_frequency();
}

//! Set the frequency of the given channel
/*!
  \param ichan the index of the channel to be set
  \param frequency the frequency of the given channel in MHz
*/
void Pulsar::Integration::set_frequency (unsigned ichan, double frequency)
{
  if (ichan < 0 || ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::set_frequency",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  for (unsigned ipol=0; ipol<get_npol(); ipol++)
    profiles[ipol][ichan]->set_centre_frequency (frequency);
}

//! Get the weight of the given channel
/*!
  \param ichan the index of the channel to get
  \return the weight of the given channel
*/
float Pulsar::Integration::get_weight (unsigned ichan) const
{
  if (ichan < 0 || ichan>=get_nchan() || get_npol() < 1)
    return 0;

  return profiles[0][ichan]->get_weight();
}

//! Set the weight of the given channel
/*!
  \param ichan the index of the channel to be set
  \param weight the weight of the given channel
*/
void Pulsar::Integration::set_weight (unsigned ichan, float weight)
{
  if (ichan < 0 || ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::set_weight",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  for (unsigned ipol=0; ipol<get_npol(); ipol++)
    profiles[ipol][ichan]->set_weight (weight);
}


void Pulsar::Integration::fold (unsigned nfold)
{
  if (get_npol()<1 || get_nchan()<1)
    return;

  try {
    for (unsigned ipol=0; ipol<get_npol(); ipol++)
      for (unsigned ichan=0; ichan<get_nchan(); ichan++)
	profiles[ipol][ichan] -> fold (nfold);

    set_nbin( profiles[0][0] -> get_nbin() );
  }
  catch (Error& error) {
    throw error += "Integration::fold";
  }
}

void Pulsar::Integration::bscrunch (unsigned nscrunch)
{
  if (get_npol()<1 || get_nchan()<1)
    return;

  if (verbose)
    cerr << "Integration::bscrunch npol=" << get_npol() 
	 << " nchan=" << get_nchan() << endl;

  try {
    for (unsigned ipol=0; ipol<get_npol(); ipol++)
      for (unsigned ichan=0; ichan<get_nchan(); ichan++)
	profiles[ipol][ichan] -> bscrunch (nscrunch);

    set_nbin ( profiles[0][0] -> get_nbin() );
  }
  catch (Error& error) {
    throw error += "Integration::bscrunch";
  }
}
    
void Pulsar::Integration::pscrunch()
{
  if (verbose)
    cerr << "Integration::pscrunch " << Signal::state_string(get_state())
	 << endl;

  if (get_state() == Signal::Coherence || get_state() == Signal::PPQQ) {

    if (get_npol() < 2)
      throw Error (InvalidState, "Integration::pscrunch", "npol < 2");

    for (unsigned ichan=0; ichan < get_nchan(); ichan++)
      *(profiles[0][ichan]) += *(profiles[1][ichan]);

    set_state (Signal::Intensity);
  }

  else if (get_state() == Signal::Stokes)
    set_state (Signal::Intensity);

  resize (1);
} 

void Pulsar::Integration::rotate (double time)
{
  double pfold = get_folding_period ();

  if (pfold <= 0.0)
    throw Error (InvalidParam, "Integration::rotate",
		 "folding period=%lf", pfold);

  try {
    for (unsigned ipol=0; ipol<get_npol(); ipol++)
      for (unsigned ichan=0; ichan<get_nchan(); ichan++)
	profiles[ipol][ichan] -> rotate (time/pfold);

    set_mid_time (get_mid_time() + time);
  }
  catch (Error& error) {
    throw error += "Integration::rotate";
  }
}

MJD Pulsar::Integration::get_start_time () const
{
  return get_mid_time() - .5 * get_duration(); 
}

MJD Pulsar::Integration::get_end_time () const
{ 
  return get_mid_time() + .5 * get_duration();
}

void Pulsar::Integration::uniform_weight ()
{
  for (unsigned ipol=0; ipol < get_npol(); ipol++)
    for (unsigned ichan=0; ichan < get_nchan(); ichan++)
      profiles[ipol][ichan] -> set_weight (1.0);
}
