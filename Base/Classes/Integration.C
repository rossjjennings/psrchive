/***************************************************************************
 *
 *   Copyright (C) 2002-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"

#include "Pulsar/IntegrationExtension.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/IntegrationTI.h"
#include "Pulsar/Profile.h"

#include "Pulsar/AuxColdPlasma.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"

#include "Error.h"
#include "typeutil.h"
#include "debug.h"

#include <algorithm>

using namespace std;

bool Pulsar::Integration::verbose = false;

//! Return the number of extensions available
unsigned Pulsar::Integration::get_nextension () const
{
  clean_dangling (extension);
  return extension.size ();
}

Pulsar::Integration::Extension::Extension (const char* _name)
{
  extension_name = _name;
}

Pulsar::Integration::Extension::~Extension ()
{
}

string Pulsar::Integration::Extension::get_extension_name () const
{
  return extension_name;
}

string Pulsar::Integration::Extension::get_short_name () const
{
  return extension_name;
}

const Pulsar::Archive* 
Pulsar::Integration::Extension::get_parent (const Integration* subint) const
{
  if (!subint->parent)
    throw Error (InvalidState, "Pulsar::Integration::Extension::get_parent",
		 "no parent");

  return subint->parent;
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
  unsigned index = find( extension, ext );

  if (index < extension.size())  {
    if (verbose)
      cerr << "Pulsar::Integration::add_extension replacing" << endl;
    extension[index] = ext;
  }
  else {
    if (verbose)
      cerr << "Pulsar::Integration::add_extension appending" << endl;
    extension.push_back(ext);
  }
}

//! Return a comma-separated list of the short names of all extensions
std::string Pulsar::Integration::list_extensions () const
{
  string retval;

  unsigned next = get_nextension();

  for (unsigned iext=0; iext<next; iext++)
  {
    if (iext>0)
      retval += ",";
    retval += get_extension(iext)->get_short_name();
  }

  return retval;
}

//! Add or remove extensions with the specified short name
void Pulsar::Integration::edit_extensions (const std::string& name)
{
  string ext = name.substr(1);

  if (name[0] == '+')
    add_extension( Integration::Extension::factory(ext) );
  
  else if (name[0] == '-')
  {
    unsigned next = get_nextension();

    for (unsigned i=0; i<next; i++)
    if (get_extension(i)->get_extension_name() == ext
	|| get_extension(i)->get_short_name() == ext)
      delete get_extension(i);
  }
  else
    throw Error (InvalidParam, "Pulsar::Integration::edit_extensions",
		 "command starts with neither '+' nor '-'");
}

static unsigned instance_count = 0;

unsigned Pulsar::Integration::get_instance_count ()
{
  return instance_count;
}

Pulsar::Integration::Integration ()
{
  DEBUG("Integration ctor this=" << this << " instances=" << instance_count);

  zero_phase_aligned = false;
  instance_count ++;
  expert_interface = new Expert (this);
}

//! Provide access to the expert interface
Pulsar::Integration::Expert* Pulsar::Integration::expert ()
{
  return expert_interface; 
}

//! Provide access to the expert interface
const Pulsar::Integration::Expert* Pulsar::Integration::expert () const
{
  return expert_interface; 
}

Pulsar::Integration::Integration (const Integration& subint)
{
  throw Error (Undefined, "Integration copy constructor",
	       "sub-classes must define copy constructor");
}

Pulsar::Integration& Pulsar::Integration::operator= (const Integration& subint)
{
  copy (&subint);
  return *this;
}

void Pulsar::Integration::zero ()
{
  for (unsigned ichan = 0; ichan < get_nchan(); ichan++)
    for (unsigned ipol = 0; ipol < get_npol(); ipol++)
      get_Profile (ipol, ichan)->zero();
}

Pulsar::Integration::~Integration ()
{
  DEBUG("Integration dtor this=" << this << " instances=" << instance_count);
  instance_count --;
}


//! Return a text interface that can be used to access this instance
Pulsar::Integration::Interface* Pulsar::Integration::get_interface ()
{
  return new Interface (this);
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
  Copy the common information from another Integration
*/
void Pulsar::Integration::copy (const Integration* subint, bool management)
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::copy entered; management="
	 << management << endl;

  if (this == subint)
    return;

  unsigned npol = subint->get_npol();
  unsigned nchan = subint->get_nchan();

  resize (npol, nchan, subint->get_nbin());

  // the following loop should copy everything but the strategy
  bool clone_strategy = false;

  for (unsigned ipol=0; ipol<npol; ipol++)
    for (unsigned ichan=0; ichan<nchan; ichan++)
      profiles[ipol][ichan]->copy(subint->profiles[ipol][ichan], clone_strategy);

  // Using a Reference::To<Extension> ensures that the cloned
  // Extension will be deleted if the derived class chooses not to
  // manage it.

  if (verbose)
    cerr << "Pulsar::Integration::copy " << subint->get_nextension() << " Extensions" << endl;

  extension.resize (0);

  for (unsigned iext=0; iext < subint->get_nextension(); iext++)
  {
    if (verbose)
      cerr << "Pulsar::Integration::copy clone " 
	   << subint->get_extension(iext)->get_extension_name() << endl;

    Reference::To<Extension> ext = subint->get_extension(iext)->clone();
    add_extension (ext);
  }

  set_epoch ( subint->get_epoch());
  set_duration ( subint->get_duration());
  set_folding_period ( subint->get_folding_period() );
  set_gate_duty_cycle ( subint->get_gate_duty_cycle() );

  if (management)
    parent = subint->parent;

  if (subint->orphaned)
    orphaned = new Meta( *(subint->orphaned) );

  zero_phase_aligned = false;
}

//! Connect to a new parent archive (also useful after cloning)
void Pulsar::Integration::adopt (const Archive* archive)
{
  if (verbose)
    cerr << "Pulsar::Integration::adopt new parent" << endl;

  orphaned = 0;
  parent = archive;
}

void Pulsar::Integration::range_check (unsigned ipol, unsigned ichan) const
{
  if (ipol >= get_npol())
    throw Error (InvalidRange, "Integration::get_Profile",
	         "ipol=%u >= npol=%u", ipol, get_npol());

  if (ipol >= profiles.size())
    throw Error (InvalidRange, "Integration::get_Profile",
                 "ipol=%u >= size=%u", ipol, profiles.size());

  if (ichan >= get_nchan())
    throw Error (InvalidRange, "Integration::get_Profile",
                 "ichan=%u >= nchan=%u", ichan, get_nchan());

  if (ichan >= profiles[ipol].size())
    throw Error (InvalidRange, "Integration::get_Profile",
                 "ichan=%u >= size=%u", ichan, profiles[ipol].size());
}

Pulsar::Profile*
Pulsar::Integration::get_Profile (unsigned ipol, unsigned ichan)
{
  if (range_checking_enabled) 
    range_check (ipol, ichan); 

  return profiles[ipol][ichan];
}

const Pulsar::Profile*
Pulsar::Integration::get_Profile (unsigned ipol, unsigned ichan) const
{
  if (range_checking_enabled)
    range_check (ipol, ichan);

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
  if (ichan>=get_nchan() || get_npol() < 1)
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
  if (ichan>=get_nchan() || get_npol() < 1)
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
  if (ichan>=get_nchan())
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
  if (ichan>=get_nchan() || get_npol() < 1)
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
  if (ichan>=get_nchan())
    throw Error (InvalidRange, "Integration::set_weight",
		 "ichan=%d nchan=%d", ichan, get_nchan());

  for (unsigned ipol=0; ipol<get_npol(); ipol++)
    profiles[ipol][ichan]->set_weight (weight);
}

//! Get the telescope name
std::string Pulsar::Integration::get_telescope() const
try {
  if (orphaned)
    return orphaned->get_telescope ();
  return parent->get_telescope ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_telescope";
}

//! Get the source coordinates
sky_coord Pulsar::Integration::get_coordinates() const
try {
  if (orphaned)
    return orphaned->get_coordinates ();
  return parent->get_coordinates ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_coordinates";
}

//! Get the centre frequency (in MHz)
double Pulsar::Integration::get_centre_frequency() const
try {
  if (orphaned)
    return orphaned->get_centre_frequency ();
  return parent->get_centre_frequency ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_centre_frequency";
}

//! Get the bandwidth (in MHz)
double Pulsar::Integration::get_bandwidth() const
try {
  if (orphaned)
    return orphaned->get_bandwidth ();
  return parent->get_bandwidth ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_bandwidth";
}

//! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
double Pulsar::Integration::get_dispersion_measure () const
try {
  if (orphaned)
    return orphaned->get_dispersion_measure ();
  return parent->get_dispersion_measure ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_dispersion_measure";
}
    
//! Inter-channel dispersion delay has been removed
bool Pulsar::Integration::get_dedispersed () const
try {
  if (orphaned)
    return orphaned->get_dedispersed ();
  return parent->get_dedispersed ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_dedispersed";
}

//! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
double Pulsar::Integration::get_rotation_measure () const
try {
  if (orphaned)
    return orphaned->get_rotation_measure ();
  return parent->get_rotation_measure ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_rotation_measure";
}

//! Data has been corrected for ISM faraday rotation
bool Pulsar::Integration::get_faraday_corrected () const
try {
  if (orphaned)
    return orphaned->get_faraday_corrected ();
  return parent->get_faraday_corrected ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_faraday_corrected";
}


double Pulsar::Integration::get_effective_dispersion_measure () const try
{
  double dm = 0;

  if (! get_dedispersed())
    dm += get_dispersion_measure ();

  if (! get_auxiliary_dispersion_corrected())
  {
    if (verbose)
      cerr << "Integration::get_effective_dispersion_measure"
              " aux dm not corrected" << endl;

    const AuxColdPlasmaMeasures* aux = get<AuxColdPlasmaMeasures>();
    if (aux)
    {
      cerr << "Integration::get_effective_dispersion_measure"
              " aux dm = " << aux->get_dispersion_measure() << endl;

      dm += aux->get_dispersion_measure();
    }
  }
  return dm;
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::get_effective_dispersion_measure";
}

//! Auxiliary inter-channel dispersion delay has been removed
bool Pulsar::Integration::get_auxiliary_dispersion_corrected () const
try
{
  if (orphaned)
    return orphaned->get_auxiliary_dispersion_corrected ();

  const AuxColdPlasma* aux = parent->get<AuxColdPlasma>();
  if (aux)
    return aux->get_dispersion_corrected();

  return false;
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::get_auxiliary_dispersion_corrected ";
}



double Pulsar::Integration::get_effective_rotation_measure () const try
{
  double rm = 0;

  if (! get_faraday_corrected())
    rm += get_rotation_measure ();

  if (! get_auxiliary_birefringence_corrected())
  {    
    const AuxColdPlasmaMeasures* aux = get<AuxColdPlasmaMeasures>();
    if (aux)
      rm += aux->get_rotation_measure();
  }
  return rm;
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::get_effective_rotation_measure";
}

//! Auxiliary inter-channel birefringence has been removed
bool Pulsar::Integration::get_auxiliary_birefringence_corrected () const
try
{
  if (orphaned)
    return orphaned->get_auxiliary_birefringence_corrected ();

  const AuxColdPlasma* aux = parent->get<AuxColdPlasma>();
  if (aux)
    return aux->get_birefringence_corrected();

  return false;
}
catch (Error& error)
{
  throw error += "Pulsar::Integration::get_auxiliary_birefringence_corrected ";
}

//! Get the feed configuration of the receiver
Signal::Basis Pulsar::Integration::get_basis () const
try {
  if (orphaned)
    return orphaned->get_basis ();
  return parent->get_basis ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_basis";
}

//! Get the polarimetric state of the profiles
Signal::State Pulsar::Integration::get_state () const
try {
  if (orphaned)
    return orphaned->get_state ();
  return parent->get_state ();
}
catch (Error& error) {
  throw error += "Pulsar::Integration::get_state";
}

//! Swap the two specified profiles
void Pulsar::Integration::swap_profiles (unsigned ipol, unsigned ichan,
					 unsigned jpol, unsigned jchan)
{
  Reference::To<Profile> temp = profiles[ipol][ichan];
  profiles[ipol][ichan] = profiles[jpol][jchan];
  profiles[jpol][jchan] = temp;
}

//! Reverse the order of frequency channels for each polarization
void Pulsar::Integration::reverse_chan ()
{
  unsigned npol = get_npol();
  for (unsigned ipol=0; ipol < npol; ipol++)
    std::reverse (profiles[ipol].begin(), profiles[ipol].end());
}

void Pulsar::Integration::update_nbin ()
{
  if (profiles.size() && profiles[0].size())
    set_nbin( profiles[0][0] -> get_nbin() );
  else
    set_nbin( 0 );
}

void Pulsar::Integration::fold (unsigned nfold) try
{
  foreach (this, &Profile::fold, nfold);
  update_nbin ();
}
catch (Error& error)
{
  throw error += "Integration::fold";
}

void Pulsar::Integration::bscrunch (unsigned nscrunch) try
{
  foreach (this, &Profile::bscrunch, nscrunch);
  update_nbin ();
}
catch (Error& error)
{
  throw error += "Integration::bscrunch";
}

void Pulsar::Integration::bscrunch_to_nbin (unsigned new_nbin) try
{
  foreach (this, &Profile::bscrunch_to_nbin, new_nbin);
  update_nbin ();
}
catch (Error& error)
{
  throw error += "Integration::bscrunch_to_nbin";
}

/*
  \pre  This method should only be called through the Archive class
  \post The calling Archive method should update state to Signal::Intensity
 */    
void Pulsar::Integration::pscrunch()
{
  Signal::State state = get_state();

  if (verbose)
    cerr << "Integration::pscrunch " << Signal::state_string(state)
	 << endl;

  const unsigned nchan = get_nchan ();

  if (state == Signal::Coherence || state == Signal::PPQQ)
  {
    if (get_npol() < 2)
      throw Error (InvalidState, "Integration::pscrunch", "npol < 2");

    for (unsigned ichan=0; ichan < nchan; ichan++)
      profiles[0][ichan] -> sum (profiles[1][ichan]);
  }

  for (unsigned ichan=0; ichan < nchan; ichan++)
    profiles[0][ichan] -> pscrunch ();

  resize (1);

  if (orphaned)
    orphaned->set_state( Signal::pscrunch(state) );
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
  foreach (this, &Profile::set_weight, new_weight);
}

