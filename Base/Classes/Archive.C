#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

#include "coord.h"

void Pulsar::Archive::init ()
{
  if (verbose)
    cerr << "Archive::init" << endl;

  model_updated = false;
}

Pulsar::Archive::Archive () 
{ 
  if (verbose)
    cerr << "Archive::null constructor" << endl;

  init(); 
}

Pulsar::Archive::Archive (const Archive& archive)
{
  throw Error (Undefined, "Archive copy constructor",
	       "sub-classes must define copy constructor");
}

Pulsar::Archive& Pulsar::Archive::operator = (const Archive& archive)
{
  copy (archive);
  return *this;
}

Pulsar::Archive::~Archive () 
{ 
  if (verbose)
    cerr << "Archive::destructor" << endl;

  for (unsigned isub=0; isub<subints.size(); isub++)
    delete subints[isub];
}

void Pulsar::Archive::copy (const Archive& archive)
{
  if (verbose)
    cerr << "Archive::copy" << endl;

  if (this == &archive)
    return;

  // set attributes
  resize (archive.get_nsubint(), archive.get_npol(),
	  archive.get_nchan(), archive.get_nbin());

  for (unsigned isub=0; isub<archive.get_nsubint(); isub++)
    subints[isub] -> copy (*(archive.subints[isub]));

  ephemeris = archive.ephemeris;
  model = archive.model;

  // set virtual attributes
  set_telescope_code( archive.get_telescope_code() );
  set_basis( archive.get_basis() );
  set_type( archive.get_type() );
  set_source( archive.get_source() );

  set_bandwidth( archive.get_bandwidth() );
  set_centre_frequency( archive.get_centre_frequency() );
  set_state( archive.get_state() );
  set_dispersion_measure( archive.get_dispersion_measure() );

  set_feedangle_corrected( archive.get_feedangle_corrected() );
  set_iono_rm_corrected( archive.get_iono_rm_corrected() );
  set_ism_rm_corrected( archive.get_ism_rm_corrected() );
  set_parallactic_corrected( archive.get_parallactic_corrected() );
}

/*!
  \param subint the index of the requested Integration
  \return pointer to Integration instance
*/
Pulsar::Integration* Pulsar::Archive::get_Integration (unsigned subint)
{
  if (subint < subints.size())
    return subints[subint];

  throw Error (InvalidRange, "Archive::get_Integration",
	       "isubint=%u nsubint=%u", subint, subints.size());
}

const Pulsar::Integration* 
Pulsar::Archive::get_Integration (unsigned subint) const
{
  if (subint < subints.size())
    return subints[subint];

  throw Error (InvalidRange, "Archive::get_Integration",
	       "isubint=%u nsubint=%u", subint, subints.size());
}
//! Return a pointer to the Profile
/*!
  \param subint the index of the requested Integration
  \param pol the index of the requested polarization
  \param chan the index of the requested frequency channel
  \return pointer to Profile instance
*/
Pulsar::Profile* 
Pulsar::Archive::get_Profile (unsigned subint, unsigned pol, unsigned chan)
{
  return get_Integration (subint) -> get_Profile (pol, chan);
}

const Pulsar::Profile* 
Pulsar::Archive::get_Profile (unsigned subint, unsigned pol, unsigned chan) const
{
  return get_Integration (subint) -> get_Profile (pol, chan);
}

/*!  
  This method may be useful during load.  This function assumes
  that the Integration is totally uninitialized.  As the folding
  period is unknown until the mid_time of the integration is known,
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
  Simply calls Integration::bscrunch on each element of subints
  \param nscrunch the number of phase bins to add together
  */
void Pulsar::Archive::bscrunch (unsigned nscrunch)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> bscrunch (nscrunch);

  set_nbin (subints[0]->get_nbin());
}

/*!
  Simply calls Integration::fscrunch on each element of subints
  \param nscrunch the number of frequency channels to add together
  */
void Pulsar::Archive::fscrunch (unsigned nscrunch, bool weighted_cfreq)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> fscrunch (nscrunch, weighted_cfreq);

  set_nchan (subints[0]->get_nchan());
}


/*!
  Simply calls Integration::pscrunch on each element of subints
*/
void Pulsar::Archive::pscrunch()
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> pscrunch ();

  set_npol ( subints[0] -> get_npol() );
  set_state ( subints[0] -> get_state() );
}

/*!
  Simply calls Integration::convert_state on each element of subints
*/
void Pulsar::Archive::convert_state (Signal::State state)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> convert_state (state);

  set_npol ( subints[0] -> get_npol() );
  set_state ( subints[0] -> get_state() );
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

  Phase half_turn (0.5);

  for (unsigned isub=0; isub < get_nsubint(); isub++)  {

    Integration* subint = subints[isub];

    // Rotate according to polyco prediction
    Phase phase = model.phase (subint -> get_mid_time(),
			       subint -> get_centre_frequency());

    if (verbose)
      cerr << "Archive::center phase=" << phase << endl;

    double fracturns = (half_turn - phase).fracturns();
    subint -> rotate ( fracturns * subint -> get_folding_period() );
  }
}


/*!
  \param dm the dispersion measure
  \param frequency */
void Pulsar::Archive::dedisperse (double dm, double frequency)
{

}

/*!
  \param nfold the number of sections to integrate
*/
void Pulsar::Archive::fold (unsigned nfold)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> fold (nfold);

  set_nbin (subints[0]->get_nbin());
}

/*!
  \param standard
  \retval toas
*/
void Pulsar::Archive::toas (const Archive* standard, vector<Tempo::toa>& toas)
{

}

void Pulsar::Archive::deparallactify()
{

}

void Pulsar::Archive::parallactify()
{

}

void Pulsar::Archive::invint ()
{

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
      subints[isub] -> remove_baseline (phase);

  }
  catch (Error& error) {
    throw error += "Archive::remove_baseline";
  }
}


void Pulsar::Archive::rotate (double time)
{

}


/*!
  \pre Archive polarimetric state must represent Stokes IQUV
  \pre The baseline must have been removed.
  \param rotation_measure
  \param rm_iono
*/
void Pulsar::Archive::defaraday (double rotation_measure, double rm_iono)
{

}

void Pulsar::Archive::set_ephemeris (const psrephem& new_ephemeris)
{
  ephemeris = new_ephemeris;
  update_model ();
}

void Pulsar::Archive::set_model (const polyco& new_model)
{
  if (!good_model (new_model))
    throw Error (InvalidParam, "Archive::set_model",
		 "supplied model does not span Integrations");

  // swap the old with the new
  polyco oldmodel = model;
  model = new_model;

  if ( oldmodel.pollys.size() ) {
    if (verbose)
      cerr << "Archive::set_model correcting against the old model" << endl;

    // correct the subints against the old model
    for (unsigned isub = 0; isub < subints.size(); isub++)
      apply_model (oldmodel, subints[isub]);
  }

  // it may not be true the that supplied model was generated at runtime
  model_updated = false; 
}

void Pulsar::Archive::snr_weight ()
{

}


MJD Pulsar::Archive::start_time() const
{
  if (subints.size() < 1)
    throw Error (InvalidState, "Archive::start_time", "no subints");

  return subints[0] -> get_start_time();
}

MJD Pulsar::Archive::end_time() const
{
  if (subints.size() < 1)
    throw Error (InvalidState, "Archive::end_time", "no subints");

  return subints[0] -> get_end_time();
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

void
Pulsar::Archive::telescope_coordinates 
(float* lat, float* lon, float* ele) const
{
  int ret = telescope_coords (get_telescope_code(), lat, lon, ele);
  if (ret < 0)
    throw Error (FailedCall, "Archive::telescope_coordinates",
		 "tempo code=%c", get_telescope_code ());
}

void Pulsar::Archive::uniform_weight ()
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    subints[isub] -> uniform_weight ();
}

