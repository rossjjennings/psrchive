/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Receiver.h"

#include "Types.h"
#include "Error.h"
#include "typeutil.h"

void Pulsar::Archive::init ()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::init" << endl;

  expert_interface = new Expert (this);
  runtime_model = false;
}

//! Provide access to the expert interface
Pulsar::Archive::Expert* Pulsar::Archive::expert ()
{
  return expert_interface; 
}

Pulsar::Archive::Archive () 
{ 
  if (verbose == 3)
    cerr << "Pulsar::Archive::null constructor" << endl;

  init(); 
}

//! The Archive copy constructor must never be called, call Archive::copy
Pulsar::Archive::Archive (const Archive& archive)
{
  throw Error (Undefined, "Pulsar::Archive copy constructor",
	       "sub-classes must define copy constructor");
}


Pulsar::Archive::~Archive () 
{ 
  if (verbose == 3)
    cerr << "Pulsar::Archive::destructor" << endl;
}

string Pulsar::Archive::get_revision (const char* revision)
{
  char buf1 [64];
  char buf2 [64];
  sscanf (revision, "%s %s", buf1, buf2);
  return buf2;
}

//! operator =
Pulsar::Archive&
Pulsar::Archive::operator = (const Archive& a)
{
  copy (a); 
  return *this;
}

//! Return a null-constructed instance of the derived class
Pulsar::Archive* Pulsar::Archive::new_Archive (const string& class_name)
{
  if (Agent::registry.size() == 0)
    throw Error (InvalidState, "Pulsar::Archive::new_Archive",
		 "no Agents loaded");

  for (unsigned agent=0; agent<Agent::registry.size(); agent++)
    if (Agent::registry[agent]->get_name() == class_name)
      return Agent::registry[agent]->new_Archive();

  throw Error (InvalidParam, "Pulsar::Archive::new_Archive",
		 "no Agent named '" + class_name + "'");
}

void Pulsar::Archive::agent_report ()
{
  Agent::report ();
}

void Pulsar::Archive::agent_list ()
{
  cout << Agent::get_list () << endl;
}

template<class T> void clean_dangling (vector<T>& data)
{
  for (unsigned i=0; i<data.size(); i++)
    if (!data[i])
      data.erase( data.begin() + i );
}

//! Return the number of extensions available
unsigned Pulsar::Archive::get_nextension () const
{
  clean_dangling (const_cast<Archive*>(this)->extension);
  return extension.size ();
}

Pulsar::Archive::Extension::Extension (const char* _name)
{
  extension_name = _name;
}

Pulsar::Archive::Extension::~Extension ()
{
}

string Pulsar::Archive::Extension::get_extension_name () const
{
  return extension_name;
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Archive base class simply calls this method. */
const Pulsar::Archive::Extension*
Pulsar::Archive::get_extension (unsigned iext) const
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Archive::get_extension",
		 "index=%d >= nextension=%d", iext, extension.size());

  if ( !extension[iext] )
    return 0;

  return extension[iext];
}

/*! Simply calls get_extension const */
Pulsar::Archive::Extension*
Pulsar::Archive::get_extension (unsigned iext)
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Archive::get_extension",
		 "index=%d >= nextension=%d", iext, extension.size());

  if ( !extension[iext] )
    return 0;

  return extension[iext];
}

/* Unless a Receiver Extension is present, this method assumes that the
   signal basis is linear. */
Signal::Basis Pulsar::Archive::get_basis () const
{
  const Receiver* receiver = get<Receiver>();
  if (receiver)
    return receiver->get_basis();
  else
    return Signal::Linear;
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Archive base class simply calls this method. */
void Pulsar::Archive::add_extension (Extension* ext)
{

  if (dynamic_cast<Pulsar::IntegrationOrder*>(ext))

    // Test to see if the archive has already had its Integrations
    // re-ordered. At the moment the code only supports doing this
    // once, otherwise each IntegrationOrder class would have to know
    // how to convert from all of its fellows and this would get very
    // complicated... AWH 29/12/2003
    
    for (unsigned i = 0; i < extension.size(); i++)
      if (dynamic_cast<Pulsar::IntegrationOrder*>(extension[i].get()))
	throw Error(InvalidState, "Archive::add_extension",
		    "Stacking IntegrationOrder Extensions is not supported");
  
  // If we reach here, there are no IntegrationOrder conflicts.
  // Continue as normal... AWH 29/12/2003
  
  unsigned index = find (extension, ext);

  if (index < extension.size())  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::add_extension replacing" << endl;
    extension[index] = ext;
  }
  else {
    if (verbose == 3)
      cerr << "Pulsar::Archive::add_extension appending" << endl; 
    extension.push_back(ext);
  }
}

void Pulsar::Archive::refresh()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::refresh" << endl;
  
  IntegrationManager::resize(0);
  
  load_header (__load_filename.c_str());
}

void Pulsar::Archive::update()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::update" << endl;

  load_header (__load_filename.c_str());
}


//! Return a pointer to the Profile
/*!
  \param sub the index of the requested Integration
  \param pol the index of the requested polarization
  \param chan the index of the requested frequency channel
  \return pointer to Profile instance
*/
Pulsar::Profile* 
Pulsar::Archive::get_Profile (unsigned sub, unsigned pol, unsigned chan)
{
  return get_Integration (sub) -> get_Profile (pol, chan);
}

const Pulsar::Profile* 
Pulsar::Archive::get_Profile (unsigned sub, unsigned pol, unsigned chan) const
{
  return get_Integration (sub) -> get_Profile (pol, chan);
}

Pulsar::Integration* Pulsar::Archive::load_Integration (unsigned isubint)
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::load_Integration" << endl;

  if (!__load_filename.length())
    throw Error (InvalidState, "Pulsar::Archive::load_Integration",
                 "internal error: instance not loaded from file");

  return load_Integration (__load_filename.c_str(), isubint);
}


/*!
  Useful wrapper for Archive::bscrunch
*/
void Pulsar::Archive::bscrunch_to_nbin (unsigned new_nbin)
{
  if (new_nbin <= 0)
    throw Error (InvalidParam, "Pulsar::Archive::bscrunch_to_nbin",
		 "Invalid nbin request");
  else if (get_nbin() < new_nbin)
    throw Error (InvalidParam, "Pulsar::Archive::bscrunch_to_nbin",
		 "Archive has too few bins (%d) to bscrunch to %d bins",
		 get_nbin(), new_nbin);
  else
    bscrunch(get_nbin() / new_nbin);
}

/*!
  Simply calls Integration::bscrunch for each Integration
  \param nscrunch the number of phase bins to add together
  */
void Pulsar::Archive::bscrunch (unsigned nscrunch)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> bscrunch (nscrunch);

  set_nbin (get_Integration(0)->get_nbin());
}


/*!
  Simply calls Integration::pscrunch for each Integration
*/
void Pulsar::Archive::pscrunch()
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> pscrunch ();

  set_npol( 1 );
  set_state( Signal::Intensity );
}


/*!
  Uses the polyco model, as well as the centre frequency and mid-time of
  each Integration to determine the predicted pulse phase.
 */
void Pulsar::Archive::centre ()
{
  // this function doesn't work for things without polycos
  if (get_type () != Signal::Pulsar)
    return;

  if (!model)
    throw Error (InvalidState, "Pulsar::Archive::centre",
		 "Pulsar observation with no polyco");

  Phase half_turn (0.5);

  for (unsigned isub=0; isub < get_nsubint(); isub++)  {

    Integration* subint = get_Integration(isub);

    // Rotate according to polyco prediction
    Phase phase = model->phase (subint -> get_epoch(),
				subint -> get_centre_frequency());

    if (verbose == 3)
      cerr << "Pulsar::Archive::center phase=" << phase << endl;

    double fracturns = (half_turn - phase).fracturns();
    subint -> rotate ( fracturns * subint -> get_folding_period() );
  }
}




/*!
  \param nfold the number of sections to integrate
*/
void Pulsar::Archive::fold (unsigned nfold)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> fold (nfold);

  set_nbin (get_Integration(0)->get_nbin());
}

void Pulsar::Archive::rotate (double time)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> rotate (time);
}

void Pulsar::Archive::rotate_phase (double phase)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> rotate_phase (phase);
}

/*!  
  The dedisperse method removes the dispersive delay between
  each frequency channel and that of the reference frequency
  defined by get_centre_frequency.
*/
void Pulsar::Archive::dedisperse () try {

  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> dedisperse ();

  set_dedispersed (true);

}
catch (Error& error) {
  throw error += "Pulsar::Archive::dedisperse";
}


/*! \param new_ephemeris the ephemeris to be installed
    \param update create a new polyco for the new ephemeris
 */
void Pulsar::Archive::set_ephemeris (const psrephem& new_ephemeris, bool update)
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::set_ephemeris" << endl;

  ephemeris = new psrephem (new_ephemeris);

  if (get_source() != ephemeris->psrname()) {

    // a CAL observation shouldn't have an ephemeris
    // no need for the removed test

    string temp1 = get_source();
    string temp2 = ephemeris->psrname();

    bool change = false;

    if (temp1.length() > temp2.length())
      change = temp1.substr(1,temp1.length()) != temp2;
    else   
      change = temp2.substr(1,temp2.length()) != temp1;

    if (change) {
      if (verbose == 3)
        cerr << "Archive::set_ephemeris Informative Notice:\n" 
             << "   Source name will be updated to match new ephemeris\n"
             << "   New name: " << temp2 << endl;
      set_source(temp2);
    }
  }

  if (get_dispersion_measure() != ephemeris->get_dm()) {
    if (verbose == 3)
      cerr << "Archive::set_ephemeris Informative Notice:\n" 
	   << "   Dispersion measure will be updated to match new ephemeris\n"
	   << "   Old DM = " << get_dispersion_measure() << endl
	   << "   New DM = " << ephemeris->get_dm() << endl;
    set_dispersion_measure(ephemeris->get_dm());
  }

  if (update)  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::set_ephemeris update polyco" << endl;
    update_model ();
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::set_ephemeris exit" << endl;
}

const psrephem Pulsar::Archive::get_ephemeris () const
{
  if (!ephemeris)
    throw Error (InvalidState,"Pulsar::Archive::get_ephemeris","no ephemeris");

  return *ephemeris;
}

void Pulsar::Archive::set_model (const polyco& new_model)
{
  if (!good_model (new_model))
    throw Error (InvalidParam, "Pulsar::Archive::set_model",
		 "supplied model does not span Integrations");

  // swap the old with the new
  Reference::To<polyco> oldmodel = model;
  model = new polyco (new_model);

  if (verbose == 3)
    cerr << "Pulsar::Archive::set_model apply the new model" << endl;

  // correct Integrations against the old model
  for (unsigned isub = 0; isub < get_nsubint(); isub++)
    apply_model (get_Integration(isub), oldmodel.ptr());

  // it may not be true the that supplied model was generated at runtime
  runtime_model = false; 
}

const polyco Pulsar::Archive::get_model () const
{
  if (!model)
    throw Error (InvalidState, "Pulsar::Archive::get_model", "no model");
		 
  return *model;
}

void Pulsar::Archive::snr_weight ()
{
  throw Error (InvalidState, "Pulsar::Archive::snr_weight", "not implemented");
}

MJD Pulsar::Archive::start_time() const
{
  if (get_nsubint() < 1)
    throw Error (InvalidState, "Pulsar::Archive::start_time",
		 "no Integrations");

  return get_Integration(0) -> get_start_time();
}

MJD Pulsar::Archive::end_time() const
{
  if (get_nsubint() < 1)
    throw Error (InvalidState, "Pulsar::Archive::end_time",
		 "no Integrations");

  return get_Integration(get_nsubint()-1) -> get_end_time();
}

double Pulsar::Archive::integration_length() const
{
  double total = 0.0;

  for (unsigned i = 0; i < get_nsubint(); i++) {
    total = total + get_Integration(i) -> get_duration();
  }

  return total;

}

bool Pulsar::Archive::type_is_cal () const
{
  return
    get_type() == Signal::PolnCal ||
    get_type() == Signal::FluxCalOn ||
    get_type() == Signal::FluxCalOff;
}


void Pulsar::Archive::uniform_weight (float new_weight)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> uniform_weight (new_weight);
}

//! A dsp::Transformation into an Archive must be able to call this
bool Pulsar::Archive::state_is_valid(string& reason) const{
  return Signal::valid_state(get_state(),1,get_npol(),reason);
}

 
void Pulsar::Archive::get_profile_power_spectra (float gamma)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> get_profile_power_spectra (gamma);

  set_nbin (get_Integration(0)->get_nbin());
}

bool Pulsar::range_checking_enabled = true;
