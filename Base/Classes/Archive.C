#include "Types.h"

#include "unistd.h"
#include "cpgplot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"

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
Pulsar::Archive* Pulsar::Archive::new_Archive (const string class_name)
{
  Agent::init ();

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
  // Test to see if the archive has already had its Integrations
  // re-ordered. At the moment the code only supports doing this
  // once, otherwise each IntegrationOrder class would have to
  // know how to convert from all of its fellows and this would
  // get very complicated... AWH 29/12/2003
  
  bool has_alt_order = false;
  
  for (unsigned i = 0; i < extension.size(); i++) {
    if (dynamic_cast<Pulsar::IntegrationOrder*>(extension[i].get()))
      has_alt_order = true;
  }
  
  if (dynamic_cast<Pulsar::IntegrationOrder*>(ext) && has_alt_order)
    throw Error(InvalidState, "Archive::add_extension",
		"Stacking IntegrationOrder Extensions is not supported");
  
  // If we reach here, there are no IntegrationOrder conflicts.
  // Continue as normal... AWH 29/12/2003

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
void Pulsar::Archive::remove_baseline (float phase, float dc)
{
  try {

    if (phase == -1.0)
      phase = find_min_phase (dc);

    for (unsigned isub=0; isub < get_nsubint(); isub++)
      get_Integration(isub) -> remove_baseline (phase, dc);

  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::remove_baseline";
  }
}

double Pulsar::Archive::find_best_period (){

  Pulsar::Plotter myplotter;
  static int loop = 0;
  // Firstly determine the MIDtime
  MJD midtime = (get_Integration(get_nsubint()-1)->get_epoch()+
		 get_Integration(0)->get_epoch())/2.0;

  // Now get the centre folding period
  double centre_period = get_Integration(0)->get_folding_period();

  // loop over trial periods and remember best snr & p
  float best_snr;
  double best_period;

  best_snr = 0.0;
  best_period = centre_period;
  
  // Period limits
  MJD t_on_2 = midtime-get_Integration(0)->get_epoch();
  double p_l = centre_period - 0.5 * pow(centre_period,2.0)/
    t_on_2.in_seconds();
  double p_u = centre_period + 0.5 * pow(centre_period,2.0)/
    t_on_2.in_seconds();

  double dt = t_on_2.in_seconds();
  double dp = pow(centre_period,2.0)/dt/(double)(get_nbin()/2);

  Reference::To<Archive> scrunched_copy = clone(); 
  // make sure appropriate scrunching
  scrunched_copy->fscrunch(0);
  scrunched_copy->pscrunch();
  for (double trial_p = p_l;trial_p<p_u;trial_p+=dp){
    // Copy the archive into workspace

    Reference::To<Archive> acopy = scrunched_copy->clone();
    acopy->new_folding_period(trial_p);
    //    cpgbbuf();
    acopy->tscrunch(0);
    // find the edges and plot them in red.
    int rise=0,fall=0;
    acopy->get_Profile(0,0,0)->find_peak_edges(rise,fall);
    /*    cpgbbuf();
    cpgeras();
    myplotter.singleProfile(acopy);
    cpgsci(2);
    rise = rise % get_nbin();
    fall = fall % get_nbin();
    cpgmove((float)rise/get_nbin(),0.0);
    cpgdraw((float)rise/get_nbin(),1000000.0);
    cpgsci(3);
    cpgmove((float)fall/get_nbin(),0.0);
    cpgdraw((float)fall/get_nbin(),1000000.0);
    cpgebuf();
    */
    float trial_snr = acopy->get_Profile(0,0,0)->snr();
    if (trial_snr>best_snr){
      best_period = trial_p;
      best_snr = trial_snr;
    }
  }
  return(best_period);
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

/*! \param new_ephemeris the ephemeris to be installed
    \param update create a new polyco for the new ephemeris
 */
void Pulsar::Archive::set_ephemeris (const psrephem& new_ephemeris, bool update)
{
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
      cerr << "Archive::set_ephemeris Informative Notice:\n" 
           << "   Source name will be updated to match new ephemeris\n"
           << "   New name: " << temp2 << endl;
      set_source(temp2);
    }
  }

  if (get_dispersion_measure() != ephemeris->get_dm()) {
    cerr << "Archive::set_ephemeris Informative Notice:\n" 
	 << "   Dispersion measure will be updated to match new ephemeris\n"
	 << "   Old DM = " << get_dispersion_measure() << endl
	 << "   New DM = " << ephemeris->get_dm() << endl;
    set_dispersion_measure(ephemeris->get_dm());
  }

  if (update)
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

  if (verbose)
    cerr << "Pulsar::Archive::set_model model set" << endl;

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

//! A dsp::Transformation into an Archive must be able to call this
bool Pulsar::Archive::state_is_valid(string& reason) const{
  return Signal::valid_state(get_state(),1,get_npol(),reason);
}

// Rotates subints as if a new period was being used.
// Intended for use with non-polyco data.
void Pulsar::Archive::new_folding_period(double trial_p){
    MJD mt = (get_Integration(get_nsubint()-1)->get_epoch()+
		 get_Integration(0)->get_epoch())/2.0;
    double cp = get_Integration(0)->get_folding_period();
    for (int i=0;i<(int)get_nsubint();i++){
      MJD dMJD = mt-get_Integration(i)->get_epoch();
      double dseconds = dMJD.in_seconds() * (1.0/cp-1.0/trial_p)*cp;
      get_Integration(i)->rotate(dseconds);
    }
    // Doesn't work with   acopy->set_folding_period(trial_p);
}
