#include <assert.h>

#include "Integration.h"
#include "Profile.h"
#include "Error.h"

bool Pulsar::Integration::verbose = false;

void Pulsar::Integration::init()
{
  // currently no variables to initialize
}

Pulsar::Integration::~Integration ()
{
  if (verbose)
    cerr << "Pulsar::Integration destructor" << endl;

  for (unsigned ipol=0; ipol<profiles.size(); ipol++)
    for (unsigned ichan=0; ichan<profiles[ipol].size(); ichan++)
      delete profiles[ipol][ichan];
}

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
  */
void Pulsar::Integration::resize (int new_npol, int new_nchan, int new_nbin)
{
  int cur_npol = get_npol();
  int cur_nchan = get_nchan();
  int cur_nbin = get_nbin();

  if (new_npol == 0)
    new_npol = cur_npol;
  if (new_nchan == 0)
    new_nchan = cur_nchan;
  if (new_nbin == 0)
    new_nbin = cur_nbin;

  if (verbose)
    cerr << "Integration::resize npol=" << new_npol
	 << " nchan=" << new_nchan << " nbin=" << new_nbin << endl 
	 << " old npol=" << cur_npol
	 << " nchan=" << cur_nchan << " nbin=" << cur_nbin << endl;

  int ipol=0, ichan=0;

  for (ipol=0; ipol < cur_npol; ipol++) {
    for (ichan=new_nchan; ichan < cur_nchan; ichan++) {
      if (profiles[ipol][ichan])
	delete profiles[ipol][ichan];  profiles[ipol][ichan]=0;
    }
    if (ipol < new_npol) {
      profiles[ipol].resize(new_nchan);
      for (ichan=cur_nchan; ichan < new_nchan; ichan++)
	profiles[ipol][ichan] = new_Profile();
    }
  }

  for (ipol=new_npol; ipol < cur_npol; ipol++)
    for (ichan=0; ichan < cur_nchan; ichan++) {
      if (profiles[ipol][ichan])
	delete profiles[ipol][ichan];  profiles[ipol][ichan]=0;
    }

  profiles.resize (new_npol);

  for (ipol=cur_npol; ipol < new_npol; ipol++) {
    profiles[ipol].resize(new_nchan);
    for (ichan=0; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] = new_Profile();
  }

  for (ipol=0; ipol < new_npol; ipol++)
    for (ichan=0; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] -> resize(new_nbin);

  set_npol (new_npol);
  set_nchan (new_nchan);
  set_nbin (new_nbin);

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


Pulsar::Integration::Integration (const Integration& subint,
				  int _npol, int _nchan)
{
  if (_npol == 0)
    _npol = subint.get_npol();

  if (_nchan == 0)
    _nchan = subint.get_nchan();

  if (_npol > subint.get_npol())
    throw Error (InvalidRange, "Integration copy constructor",
		 "requested npol=%d.  npol=%d", _npol, subint.get_npol());

  if (_nchan > subint.get_nchan())
    throw Error (InvalidRange, "Integration copy constructor",
		 "requested nchan=%d.  nchan=%d", _nchan, subint.get_nchan());

  resize (_npol, _nchan, subint.get_nbin());

  for (int ipol=0; ipol<_npol; ipol++)
    for (int ichan=0; ichan<_nchan; ichan++)
      *(profiles[ipol][ichan]) = *(subint.profiles[ipol][ichan]);
}

//! copy the main information from another subint
void Pulsar::Integration::copy (const Integration& subint)
{
  set_mid_time ( subint.get_mid_time());
  set_duration ( subint.get_duration());
  set_centre_frequency ( subint.get_centre_frequency() );
  set_bandwidth ( subint.get_bandwidth() );
  set_dispersion_measure ( subint.get_dispersion_measure() );
  set_folding_period ( subint.get_folding_period() );
  set_feed_type ( subint.get_feed_type() );
  set_poln_state ( subint.get_poln_state() );
}

Pulsar::Profile* Pulsar::Integration::get_Profile (int ipol, int ichan)
{
  if (ipol < 0 || ipol>=get_npol())
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ipol=%d npol=%d", ipol, get_npol());

  if (ichan < 0 || ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::get_Profile",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  return profiles[ipol][ichan];
}

//! Get the frequency of the given channel
/*!
  \param ichan the index of the channel to get
  \return the frequency of the given channel in MHz
*/
double Pulsar::Integration::get_frequency (int ichan) const
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
void Pulsar::Integration::set_frequency (int ichan, double frequency)
{
  if (ichan < 0 || ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::set_frequency",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  for (int ipol=0; ipol<get_npol(); ipol++)
    profiles[ipol][ichan]->set_centre_frequency (frequency);
}

//! Get the weight of the given channel
/*!
  \param ichan the index of the channel to get
  \return the weight of the given channel
*/
float Pulsar::Integration::get_weight (int ichan) const
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
void Pulsar::Integration::set_weight (int ichan, float weight)
{
  if (ichan < 0 || ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::set_weight",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  for (int ipol=0; ipol<get_npol(); ipol++)
    profiles[ipol][ichan]->set_weight (weight);
}

vector<Pulsar::Profile*>& Pulsar::Integration::operator[] (Poln::Measure poln)
{
  int index = Poln::get_ipol (get_poln_state(), poln);

  if (index < 0)
    throw Error (InvalidPolnState, "Integration::operator[]");

  return profiles[index];
}

void Pulsar::Integration::fold (int nfold)
{
  if (get_npol()<1 || get_nchan()<1)
    return;

  try {
    for (int ipol=0; ipol<get_npol(); ipol++)
      for (int ichan=0; ichan<get_nchan(); ichan++)
	profiles[ipol][ichan] -> fold (nfold);

    set_nbin( profiles[0][0] -> get_nbin() );
  }
  catch (Error& error) {
    throw error += "Integration::fold";
  }
}

void Pulsar::Integration::bscrunch (int nscrunch)
{
  if (get_npol()<1 || get_nchan()<1)
    return;

  if (verbose)
    cerr << "Integration::bscrunch npol=" << get_npol() 
	 << " nchan=" << get_nchan() << endl;

  try {
    for (int ipol=0; ipol<get_npol(); ipol++)
      for (int ichan=0; ichan<get_nchan(); ichan++)
	profiles[ipol][ichan] -> bscrunch (nscrunch);

    set_nbin ( profiles[0][0] -> get_nbin() );
  }
  catch (Error& error) {
    throw error += "Integration::bscrunch";
  }
}
    
void Pulsar::Integration::pscrunch()
{
  if (get_poln_state() == Poln::Coherence || get_poln_state() == Poln::PPQQ) {

    if (get_npol() != 2)
      throw Error (InvalidState, "Integration::pscrunch", "npol != 2");

    for (int ichan=0; ichan < get_nchan(); ichan++)
      *(profiles[0][ichan]) += *(profiles[1][ichan]);

    set_poln_state (Poln::Intensity);
  }

  else if (get_poln_state() == Poln::Stokes)
    set_poln_state (Poln::Intensity);

  resize (1);
} 

void Pulsar::Integration::rotate (double time)
{
  double pfold = get_folding_period ();

  if (pfold <= 0.0)
    throw Error (InvalidParam, "Integration::rotate",
		 "folding period=%lf", pfold);

  try {
    for (int ipol=0; ipol<get_npol(); ipol++)
      for (int ichan=0; ichan<get_nchan(); ichan++)
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
