#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Error.h"
#include "typeutil.h"
#include "coord.h"

const vector<unsigned> Pulsar::Archive::none_selected;

void Pulsar::Archive::init ()
{
  if (verbose)
    cerr << "Pulsar::Archive::init" << endl;

  model_updated = false;
}

Pulsar::Archive::Archive () 
{ 
  if (verbose)
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
  if (verbose)
    cerr << "Pulsar::Archive::destructor" << endl;
}

//! Return a null-constructed instance of the derived class
Pulsar::Archive* Pulsar::Archive::new_Archive (const char* class_name)
{
  Agent::init ();

  if (Agent::registry.size() == 0)
    throw Error (InvalidState, "Pulsar::Archive::new_Archive",
		 "no Agents loaded");

  for (unsigned agent=0; agent<Agent::registry.size(); agent++)
    if (Agent::registry[agent]->get_name() == class_name)
      return Agent::registry[agent]->new_Archive();

  throw Error (InvalidParam, "Pulsar::Archive::new_Archive",
		 "no Agent named '%s'", class_name);
}

void Pulsar::Archive::agent_report ()
{
  Agent::report ();
}

void Pulsar::Archive::set_plugin_path (const char* path)
{
  Agent::plugin_path = path;
}


//! Return the number of extensions available
unsigned Pulsar::Archive::get_nextension () const
{
  return extension.size ();
}

Pulsar::Archive::Extension::Extension (const char* _name)
{
  name = _name;
}

Pulsar::Archive::Extension::~Extension ()
{
}

string Pulsar::Archive::Extension::get_name () const
{
  return name;
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Archive base class simply calls this method. */
const Pulsar::Archive::Extension*
Pulsar::Archive::get_extension (unsigned iext) const
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Archive::get_extension",
		 "index=%d >= nextension=%d", iext, extension.size());

  return extension[iext];
}

/*! Simply calls get_extension const */
Pulsar::Archive::Extension*
Pulsar::Archive::get_extension (unsigned iext)
{
  if ( iext >= extension.size() )
    throw Error (InvalidRange, "Pulsar::Archive::get_extension",
		 "index=%d >= nextension=%d", iext, extension.size());

  return extension[iext];
}

/*! Derived classes need only define this method, as the non-const version
  implemented by the Archive base class simply calls this method. */
void Pulsar::Archive::add_extension (Extension* ext)
{
  unsigned index = find( extension, typeid(ext) );

  if (index < extension.size())
    extension[index] = ext;
  else
    extension.push_back(ext);
}

void Pulsar::Archive::refresh()
{
  if (verbose)
    cerr << "Pulsar::Archive::refresh" << endl;

  IntegrationManager::resize(0);

  load_header (__load_filename.c_str());
}

void Pulsar::Archive::update()
{
  if (verbose)
    cerr << "Pulsar::Archive::update" << endl;

  load_header (__load_filename.c_str());
}


//! Return a pointer to the Profile
/*!
  \param subint the index of the requested Integration
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
  if (verbose)
    cerr << "Pulsar::Archive::load_Integration" << endl;

  if (!__load_filename.length())
    throw Error (InvalidState, "Pulsar::Archive::load_Integration",
                 "internal error: instance not loaded from file");

  return load_Integration (__load_filename.c_str(), isubint);
}

/*!  
  This method may be useful during load.  This function assumes
  that the Integration is totally uninitialized.  As the folding
  period is unknown until the epoch of the integration is known,
  Integration::set_folding_period is not called by this method.  
*/
void Pulsar::Archive::init_Integration (Integration* subint)
{
  subint -> set_centre_frequency ( get_centre_frequency() );
  subint -> set_bandwidth ( get_bandwidth() );
  subint -> set_dispersion_measure ( get_dispersion_measure() );
  subint -> set_basis ( get_basis() );
  subint -> set_state ( get_state() );
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
		 "Archive has too few bins");
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
  Simply calls Integration::fscrunch for each Integration
  \param nscrunch the number of frequency channels to add together
 */
void Pulsar::Archive::fscrunch (unsigned nscrunch, bool weighted_cfreq)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> fscrunch (nscrunch, weighted_cfreq);

  set_nchan (get_Integration(0)->get_nchan());
}

/*!
  Useful wrapper for Archive::fscrunch
*/
void Pulsar::Archive::fscrunch_to_nchan (unsigned new_chan)
{
  if (get_nchan() % new_chan != 0)
    throw Error (InvalidParam, "Pulsar::Archive::fscrunch_to_nchan",
		 "Invalid nchan request");
  else if (get_nchan() < new_chan)
    throw Error (InvalidParam, "Pulsar::Archive::fscrunch_to_nchan",
		 "Archive has too few channels");
  else
    fscrunch(get_nchan() / new_chan);
}

/*!
  Useful wrapper for Archive::tscrunch
*/
void Pulsar::Archive::tscrunch_to_nsub (unsigned new_nsub)
{
  if (new_nsub <= 0)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "Invalid nsub request");
  else if (get_nsubint() < new_nsub)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "Archive has too few subints");
  else
    tscrunch(get_nsubint() / new_nsub);
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

  set_npol ( get_Integration(0) -> get_npol() );
  set_state ( get_Integration(0) -> get_state() );
}

/*!
  Simply calls Integration::convert_state for each Integration
*/
void Pulsar::Archive::convert_state (Signal::State state)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> convert_state (state);

  set_npol ( get_Integration(0) -> get_npol() );
  set_state ( get_Integration(0) -> get_state() );
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

    if (verbose)
      cerr << "Pulsar::Archive::center phase=" << phase << endl;

    double fracturns = (half_turn - phase).fracturns();
    subint -> rotate ( fracturns * subint -> get_folding_period() );
  }
}


/*!
  \param dm the dispersion measure
  \param frequency */
void Pulsar::Archive::dedisperse (double dm, double frequency)
{
  if (get_nsubint() == 0)
    return;
  
  if (get_dedispersed()) {
    throw Error (InvalidState, "Archive::dedisperse",
		 "Archive already de-dispersed");
    return;
  }
  
  for (unsigned isub=0; isub < get_nsubint(); isub++) {
    if (dm)
      get_Integration(isub) -> set_dispersion_measure (dm);
    get_Integration(isub) -> dedisperse (frequency);
  }

  set_dedispersed();
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


void Pulsar::Archive::invint ()
{
  if (get_nsubint() == 0)
    return;
  
  remove_baseline();
  
  for (unsigned isub=0; isub < get_nsubint(); isub++) {
    get_Integration(isub) -> invint ();
  }
  
  set_state(Signal::Invariant);
}

float Pulsar::Archive::get_poln_flux (int _type) {
  
  return (get_Integration(0) -> get_poln_flux (_type));
  
}


/*!
  If phase is not specified, this method calls
  Archive::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Archive::total)
  reaches a minimum.  This phase is then used to remove the baseline from
  each of the Integrations.
  */
void Pulsar::Archive::remove_baseline (float phase)
{
  try {

    if (phase == -1.0)
      phase = find_min_phase ();

    for (unsigned isub=0; isub < get_nsubint(); isub++)
      get_Integration(isub) -> remove_baseline (phase);

  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::remove_baseline";
  }
}


void Pulsar::Archive::rotate (double time)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> rotate (time);
}


/*!
  \pre Archive polarimetric state must represent Stokes IQUV
  \pre The baseline must have been removed.
  \param rotation_measure
  \param rm_iono
*/
void Pulsar::Archive::defaraday (double rotation_measure, double rm_iono)
{
  convert_state(Signal::Stokes);
  remove_baseline();
  
  for (unsigned i = 0; i < get_nsubint(); i++)
    get_Integration(i)->defaraday(rotation_measure, rm_iono);
  
  set_ism_rm_corrected(true);
  set_iono_rm_corrected(true);
}

void Pulsar::Archive::set_ephemeris (const psrephem& new_ephemeris)
{
  ephemeris = new psrephem (new_ephemeris);
  
  if (get_source() != ephemeris->psrname()) {
    if (ephemeris->psrname() != "CAL" && ephemeris->psrname() != "cal" &&
	ephemeris->psrname() != "Cal" && ephemeris->psrname() != "JCAL" &&
	ephemeris->psrname() != "jcal") {
      string temp1 = get_source();
      string temp2 = ephemeris->psrname();
      if (temp1.length() > temp2.length()) {
	if (temp1.substr(1,temp1.length()) != temp2) {
	  cout << "Archive::set_ephemeris Informative Notice:\n" 
	       << "   Source name will be updated to match new ephemeris\n"
	       << "   New name: " << temp2 << endl;
	  set_source(temp2);
	}
      }
      else {
	if (temp2.substr(1,temp2.length()) != temp1) {
	  cout << "Archive::set_ephemeris Informative Notice:\n" 
	       << "   Source name will be updated to match new ephemeris\n"
	       << "   New name: " << temp2 << endl;
	  set_source(temp2);
	}
      }
    }
    if (get_dispersion_measure() != ephemeris->get_dm()) {
      cout << "Archive::set_ephemeris Informative Notice:\n" 
	   << "   Dispersion measure will be updated to match new ephemeris\n"
	   << "   New DM = " << ephemeris->get_dm() << endl;
      set_dispersion_measure(ephemeris->get_dm());
    }
  }

  update_model ();
}

const psrephem Pulsar::Archive::get_ephemeris ()
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

  if ( oldmodel && oldmodel->pollys.size() ) {

    if (verbose)
      cerr << "Pulsar::Archive::set_model apply the new model" << endl;

    // correct Integrations against the old model
    for (unsigned isub = 0; isub < get_nsubint(); isub++)
      apply_model (*oldmodel, get_Integration(isub));
  }

  // it may not be true the that supplied model was generated at runtime
  model_updated = false; 
}

const polyco Pulsar::Archive::get_model ()
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

/*!
  \retval lat latitude in degrees
  \retval lon longitude in degrees
  \retval ele elevation in metres
*/

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

void Pulsar::Archive::telescope_coordinates (float* lat, float* lon, 
					     float* ele) const
{
  int ret = telescope_coords (get_telescope_code(), lat, lon, ele);
  if (ret < 0)
    throw Error (FailedCall, "Pulsar::Archive::telescope_coordinates",
		 "tempo code=%c", get_telescope_code ());
}

void Pulsar::Archive::uniform_weight (float new_weight)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> uniform_weight (new_weight);
}

