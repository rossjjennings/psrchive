
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"
#include "typeutil.h"
#include <assert.h>

bool Pulsar::Integration::verbose = false;

//! Return the number of extensions available
unsigned Pulsar::Integration::get_nextension () const
{
  return extension.size ();
}

Pulsar::Integration::Extension::Extension (const char* _name)
{
  name = _name;
}

Pulsar::Integration::Extension::~Extension ()
{
}

string Pulsar::Integration::Extension::get_name () const
{
  return name;
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Integration base class simply calls this method. */
const Pulsar::Integration::Extension*
Pulsar::Integration::get_extension (unsigned iext) const
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Integration::get_extension",
		 "index=%d >= nextension=%d", iext, extension.size());

  return extension[iext];
}

/*! Simply calls get_extension const */
Pulsar::Integration::Extension*
Pulsar::Integration::get_extension (unsigned iext)
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Integration::get_extension",
		 "index=%d >= nextension=%d", iext, extension.size());

  return extension[iext];
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Integration base class simply calls this method. */
void Pulsar::Integration::add_extension (Extension* ext)
{
  unsigned index = find( extension, typeid(ext) );

  if (index < extension.size())
    extension[index] = ext;
  else
    extension.push_back(ext);
}

Pulsar::Integration::Integration ()
{
  defaradayed_rotation_measure = 0.0;
  defaradayed_centre_frequency = 0.0;
  dedispersed_dispersion_measure = 0.0;
  dedispersed_centre_frequency   = 0.0;
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

/*! Users of this method should be aware that it does not keep
  full track of all the parameters of an Integration. It is
  designed to be used for low level arithmetic style addition
  only, not fully consistent combination. The supplied tscrunch
  routines should be used for such purposes. */
void Pulsar::Integration::operator+= (const Integration& subint)
{
  float total_weight = 0.0;
  
  for (unsigned i = 0; i < subint.get_nchan(); i++) {
    total_weight += subint.get_weight(i);
  }
  
  if (total_weight <= 0.0)
    return;
  
  string reason;
  
  if (!mixable(&subint, reason))
    throw Error(InvalidParam, "Integration operator +=",
		reason);
  
  for (unsigned i = 0; i < get_nchan(); i++) {
    for (unsigned j = 0; j < get_npol(); j++) {
      *(get_Profile(j,i)) += *(subint.get_Profile(j,i));
    } 
  }
  
  double total = get_duration() + subint.get_duration();
  
  set_duration(total);
  
  return;
}

void Pulsar::Integration::zero ()
{
  vector<float> zeroes(get_nbin(), 0.0);

  for (unsigned i = 0; i < get_nchan(); i++) {
    for (unsigned j = 0; j < get_npol(); j++) {
      get_Profile(j,i)->set_amps(zeroes);
    }
  }
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

  // Using a Reference::To<Extension> ensures that the cloned
  // Extension will be deleted if the derived class chooses not to
  // manage it.

  if (verbose)
    cerr << "Pulsar::Integration::copy " << subint.get_nextension()
	 << " Extensions" << endl;

  extension.resize (0);

  for (unsigned iext=0; iext < subint.get_nextension(); iext++) {

    if (verbose)
      cerr << "Pulsar::Integration::copy clone " 
	   << subint.get_extension(iext)->get_name() << endl;

    Reference::To<Extension> ext = subint.get_extension(iext)->clone();
    add_extension (ext);

  }

  set_epoch ( subint.get_epoch());
  set_duration ( subint.get_duration());
  set_folding_period ( subint.get_folding_period() );

  archive = subint.archive;
  defaradayed_rotation_measure = subint.defaradayed_rotation_measure;
  defaradayed_centre_frequency = subint.defaradayed_centre_frequency;
  dedispersed_dispersion_measure = subint.dedispersed_dispersion_measure;
  dedispersed_centre_frequency   = subint.dedispersed_centre_frequency;

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



#if 0
//! Get the effective centre frequency of the given channel
/*!
  \param ichan the index of the channel to get

  \param dim the effect under consideration.  If dim == Signal::Phase,
  then the result will depend on if the data is dedispersed.  If dim
  == Signal::Polarization, then the result will depend on if the data
  has been corrected for Faraday rotation.

  \return the frequency of the given channel in MHz
*/
double Pulsar::Integration::get_centre_frequency (unsigned ichan,
						  Signal::Dimension dim) const
{
  if (ichan < 0 || ichan>=get_nchan() || get_npol() < 1)
    throw Error (InvalidRange, "Pulsar::Integration::get_centre_frequency");

  if (dim == Signal::Phase && get_dedispersed())
    return get_centre_frequency ();

  if (dim == Signal::Polarization && get_faraday_corrected())
    return get_centre_frequency ();

  return profiles[0][ichan]->get_centre_frequency();
}

#endif

//! Get the centre frequency of the given channel
/*!
  \param ichan the index of the channel to get
  \return the frequency of the given channel in MHz
*/
double Pulsar::Integration::get_centre_frequency (unsigned ichan) const
{
  if (ichan < 0 || ichan>=get_nchan() || get_npol() < 1)
    return 0;

  return profiles[0][ichan]->get_centre_frequency();
}

//! Set the centre frequency of the given channel
/*!
  \param ichan the index of the channel to be set
  \param frequency the frequency of the given channel in MHz
*/
void Pulsar::Integration::set_centre_frequency (unsigned ichan,
						double frequency)
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


//! Get the centre frequency (in MHz)
double Pulsar::Integration::get_centre_frequency() const
try {
  return archive->get_centre_frequency ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_centre_frequency";
}

//! Get the bandwidth (in MHz)
double Pulsar::Integration::get_bandwidth() const
try {
  return archive->get_bandwidth ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_bandwidth";
}

//! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
double Pulsar::Integration::get_dispersion_measure () const
try {
  return archive->get_dispersion_measure ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_dispersion_measure";
}
    
//! Inter-channel dispersion delay has been removed
bool Pulsar::Integration::get_dedispersed () const
try {
  return archive->get_dedispersed ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_dedispersed";
}

//! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
double Pulsar::Integration::get_rotation_measure () const
try {
  return archive->get_rotation_measure ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_rotation_measure";
}

//! Data has been corrected for ISM faraday rotation
bool Pulsar::Integration::get_faraday_corrected () const
try {
  return archive->get_faraday_corrected ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_faraday_corrected";
}

//! Get the feed configuration of the receiver
Signal::Basis Pulsar::Integration::get_basis () const
try {
  return archive->get_basis ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_basis";
}

//! Get the polarimetric state of the profiles
Signal::State Pulsar::Integration::get_state () const
try {
  return archive->get_state ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_state";
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

/*
  \pre  This method should only be called through the Archive class
  \post The calling Archive method should update state to Signal::Intensity
 */    
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

  }

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

    set_epoch (get_epoch() + time);
  }
  catch (Error& error) {
    throw error += "Integration::rotate";
  }
}

MJD Pulsar::Integration::get_start_time () const
{
  return get_epoch() - .5 * get_duration(); 
}

MJD Pulsar::Integration::get_end_time () const
{ 
  return get_epoch() + .5 * get_duration();
}

void Pulsar::Integration::uniform_weight (float new_weight)
{
  for (unsigned ipol=0; ipol < get_npol(); ipol++)
    for (unsigned ichan=0; ichan < get_nchan(); ichan++)
      profiles[ipol][ichan] -> set_weight (new_weight);
}
