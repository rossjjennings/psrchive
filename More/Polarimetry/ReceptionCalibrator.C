#include "Pulsar/ReceptionCalibrator.h"

#include "Pulsar/CorrectionsCalibrator.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Calibration/Complex2Constant.h"
#include "Calibration/SingleAxis.h"
#include "Calibration/Feed.h"
#include "Calibration/Boost.h"
#include "Calibration/Gain.h"

#include "Calibration/SingleAxisPolynomial.h"
#include "Calibration/Polynomial.h"
#include "Calibration/Tracer.h"

#include "Pauli.h"

#include <algorithm>
#include <assert.h>

/*! The Archive passed to this constructor will be used to supply the first
  guess for each pulse phase bin used to constrain the fit. */
Pulsar::ReceptionCalibrator::ReceptionCalibrator (Calibrator::Type type,
						  const Archive* archive)
{
  model_type = type;

  is_fit = false;
  is_initialized = false;

  measure_cal_V = true;
  measure_cal_Q = false;

  normalize_by_invariant = true;

  PA_min = PA_max = 0.0;

  if (archive)
    initial_observation (archive);
}

void Pulsar::ReceptionCalibrator::set_calibrators (const vector<string>& names)
{
  calibrator_filenames = names;
}


/*! 
  \param _model the type of model used to represent the receiver
  \param feed_corrections the known feed corrections transformation
*/
Pulsar::StandardModel::StandardModel (Calibrator::Type _model,
                                      Calibration::Complex2* feed_corrections)
{
  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the model of the instrument
  //

  model = _model;

  valid = true;

  instrument = new Calibration::ProductRule<Calibration::Complex2>;

#if 0
  Calibration::SingleAxisPolynomial* backend;
  backend = new Calibration::SingleAxisPolynomial (4);
  backend -> set_infit(0, false);
  convert.connect (backend, &Calibration::SingleAxisPolynomial::set_abscissa);
#else

  Calibration::Complex2* operation;
  operation = new Calibration::Rotation(Vector<double, 3>::basis(0));

  Calibration::Polynomial* poly = new Calibration::Polynomial (4);
  poly -> set_infit (0, false);
  poly -> set_argument (0, &convert);

  Calibration::ChainRule<Calibration::Complex2>* backend;
  backend = new Calibration::ChainRule<Calibration::Complex2>;

  backend -> set_model ( operation );
  backend -> set_constraint (0, poly);

#endif

  *instrument *= backend;
  time.signal.connect (&convert, &Calibration::ConvertMJD::set_epoch);

  switch (model) {

  case Calibrator::Hamaker:
    if (ReceptionCalibrator::verbose)
      cerr << "Pulsar::StandardModel Hamaker" << endl;
    polar = new Calibration::Polar;
    *instrument *= polar;
    break;

  case Calibrator::Britton:
    if (ReceptionCalibrator::verbose)
      cerr << "Pulsar::StandardModel Britton" << endl;
    physical = new Calibration::Instrument;
    *instrument *= physical;
    break;

  default:
    throw Error (InvalidParam, "Pulsar::StandardModel",
		 "unknown model code=%d", int(_model));

  }

  if (feed_corrections) {
    *instrument *= feed_corrections;
    feed_correction = feed_corrections;
  }

  equation = new Calibration::ReceptionModel;

  ReferenceCalibrator_path = 0;
  FluxCalibrator_path = 0;

  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the signal path seen by the pulsar
  //

  pulsar_path = new Calibration::ProductRule<Calibration::Complex2>;
  *pulsar_path *= instrument;
  *pulsar_path *= &parallactic;

  equation->add_transformation ( pulsar_path );
  Pulsar_path = equation->get_transformation_index ();

  time.signal.connect (&parallactic, &Calibration::Parallactic::set_epoch);

}

void Pulsar::StandardModel::add_fluxcal_backend ()
{
  if (!physical)
    throw Error (InvalidState, "Pulsar::StandardModel::add_fluxcal_backend",
		 "Cannot model flux calibrator with Hamaker model");

  Calibration::ProductRule<Calibration::Complex2>* path = 0;
  path = new Calibration::ProductRule<Calibration::Complex2>;

  fluxcal_backend = new Calibration::SingleAxis;

  *path *= fluxcal_backend;
  *path *= physical->get_feed();

  if (feed_correction)
    *path *= feed_correction;

  equation->add_transformation ( path );
  FluxCalibrator_path = equation->get_transformation_index ();
}

void Pulsar::StandardModel::add_polncal_backend ()
{
  pcal_path = new Calibration::ProductRule<Calibration::Complex2>;
  *pcal_path *= instrument;

  equation->add_transformation ( pcal_path );
  ReferenceCalibrator_path = equation->get_transformation_index ();
}

void Pulsar::StandardModel::fix_orientation ()
{

  if (physical)
    // set the orientation of the first receptor
    physical->set_infit (4, false);

  if (polar)
    // set the orientation of the last rotation
    polar->set_infit (6, false);

}

void Pulsar::StandardModel::update ()
{
  switch (model) {
  case Calibrator::Hamaker:
    polar_estimate.update (polar);
    break;
  case Calibrator::Britton:
    physical_estimate.update (physical->get_backend());
    break;
  default:
    throw Error (InvalidState, "Pulsar::StandardModel::update",
		 "unknown model");
  }

  if (fluxcal_backend)
    fluxcal_backend_estimate.update (fluxcal_backend);

}

void Pulsar::ReceptionCalibrator::initial_observation (const Archive* data)
{
  if (!data)
    throw Error (InvalidState, "ReceptionCalibrator::initial_observation",
		 "no Archive");

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::initial_observation" << endl;

  if (data->get_type() != Signal::Pulsar)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() 
		 + "' not a Pulsar observation");

  if (data->get_state() != Signal::Stokes)
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='%s' state=%s != Signal::Stokes",
		 data->get_filename().c_str(),
		 Signal::state_string(data->get_state()));

  // use the CorrectionsCalibrator class to determine applicability
  CorrectionsCalibrator corrections;

  if (! (corrections.needs_correction (data) &&
	 corrections.should_correct_vertical &&
	 corrections.must_correct_platform) )
    throw Error (InvalidParam,
		 "Pulsar::ReceptionCalibrator::initial_observation",
		 "Pulsar::Archive='" + data->get_filename() + "'\n"
		 "has been corrected for parallactic angle rotation");
		 
  if (!data->get_dedispersed ())
    cerr << "Pulsar::ReceptionCalibrator WARNING archive not dedispersed\n"
      "  Pulse phase will vary as a function of frequency channel" << endl;

  calibrator = data->clone();
  receiver = calibrator->get<Receiver>();

  Calibration::Complex2* feed = 0;
  if (receiver) {
    feed = new Calibration::Complex2Constant (receiver->get_correction());
    cerr << "Pulsar::ReceptionCalibrator adding correction transformation\n"
	"\t" << feed->evaluate() << endl;
  }

  float latitude = corrections.telescope->get_latitude().getDegrees();
  float longitude = corrections.telescope->get_longitude().getDegrees();

  sky_coord coordinates = calibrator->get_coordinates();

  unsigned nchan = calibrator->get_nchan();

  model.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    model[ichan] = new StandardModel (model_type, feed);

    if (measure_cal_Q)
      model[ichan] -> fix_orientation ();

    model[ichan]->parallactic.set_source_coordinates (coordinates);
    model[ichan]->parallactic.set_observatory_coordinates (latitude,longitude);

  }

  if (calibrator_estimate.source.size() == 0 && calibrator_filenames.size())
    load_calibrators ();

  // initialize any previously added states
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    init_estimate ( pulsar[istate] );

  start_epoch = end_epoch = data->start_time ();

  model[0]->parallactic.set_epoch (start_epoch);
  PA_max = PA_min = model[0]->parallactic.get_param (0);

}

void Pulsar::ReceptionCalibrator::load_calibrators ()
{

  for (unsigned ifile = 0; ifile < calibrator_filenames.size(); ifile++) {
    
    try {

      cerr << "Pulsar::ReceptionCalibrator::load_calibrators loading "
	   << calibrator_filenames[ifile] << endl;

      Reference::To<Archive> archive;
      archive = Pulsar::Archive::load(calibrator_filenames[ifile]);
      
      add_calibrator (archive);

    }
    catch (Error& error) {
      cerr << "Pulsar::ReceptionCalibrator::load_calibrators ERROR" 
	   << endl << error.warning() << endl;
    }

  }

  unsigned nchan = model.size();

  cerr << "Setting " << nchan << " channel receiver" << endl;
  try {
    for (unsigned ichan=0; ichan<nchan; ichan+=1)
      model[ichan]->update ();
  }
  catch (Error& error) {
    throw error += "Pulsar::ReceptionCalibrator::load_calibrators";
  }

}


//! Add the specified pulse phase bin to the set of state constraints
void Pulsar::ReceptionCalibrator::add_state (unsigned phase_bin)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_state", false);

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" 
	 << phase_bin << endl;

  for (unsigned istate=0; istate<pulsar.size(); istate++)
    if (pulsar[istate].phase_bin == phase_bin) {
      cerr << "Pulsar::ReceptionCalibrator::add_state phase bin=" << phase_bin
	   << " already in use" << endl;
      return;
    }

  pulsar.push_back( SourceEstimate (phase_bin) );

  if (calibrator)
    init_estimate( pulsar.back() );
}

void Pulsar::ReceptionCalibrator::init_estimate (SourceEstimate& estimate)
{
  unsigned nchan = calibrator->get_nchan ();
  unsigned nbin = calibrator->get_nbin ();

  if (estimate.phase_bin >= nbin)
    throw Error (InvalidRange, "Pulsar::ReceptionCalibrator::init_estimate",
		 "phase bin=%d >= nbin=%d", estimate.phase_bin, nbin);

  estimate.source.resize (nchan);
  estimate.source_guess.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    unsigned nsource = model[ichan]->equation->get_num_input();
    if (ichan==0)
      estimate.input_index = nsource;
    else if (estimate.input_index != nsource)
      throw Error (InvalidState, "Pulsar::ReceptionCalibrator::init_estimate",
		   "isource=%d != nsource=%d", estimate.input_index, nsource);

#if 0
    if (estimate.input_index == 8) {
      cerr << "Setting tracer" << endl;
      (void) new Calibration::Tracer (&(estimate.source[ichan]), 1);
    }
#endif

    model[ichan]->equation->add_input( &(estimate.source[ichan]) );
  }

}

//! Get the number of pulsar phase bin input polarization states
unsigned Pulsar::ReceptionCalibrator::get_nstate_pulsar () const
{
  return pulsar.size();
}

//! Get the total number of input polarization states
unsigned Pulsar::ReceptionCalibrator::get_nstate () const
{
  if (model.size() == 0)
    return 0;

  return model[0]->equation->get_num_input ();
}

unsigned Pulsar::ReceptionCalibrator::get_nchan () const
{
  return model.size();
}

MJD Pulsar::ReceptionCalibrator::get_epoch () const
{
  return 0.5 * (start_epoch + end_epoch);
}


//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_calibrator (const Archive* data)
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_calibrator",
		 "No Archive containing pulsar data has yet been added");

  Reference::To<ReferenceCalibrator> polncal;

  if (model_type == Calibrator::Hamaker) {

    if (verbose)
      cerr << "Pulsar::ReceptionCalibrator::add_calibrator"
	" new PolarCalibrator" << endl;
    
    polncal = new PolarCalibrator (data);
    
  }
  else if (model_type == Calibrator::Britton) {

    if (verbose)
      cerr << "Pulsar::ReceptionCalibrator::add_calibrator"
	" new SingleAxisCalibrator" << endl;
    
    polncal = new SingleAxisCalibrator (data);
    
  }
  else 
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_calibrator",
		 "unknown StandardModel type");


  polncal->set_nchan( calibrator->get_nchan() );

  add_calibrator (polncal);

}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::ReceptionCalibrator::add_observation (const Archive* data)
{
  check_ready ("Pulsar::ReceptionCalibrator::add_observation", false);

  if (!data)
    return;

  if (data->get_type() == Signal::PolnCal) {
    add_calibrator (data);
    return;
  }

  if (!calibrator)
    initial_observation (data);

  string reason;
  if (!calibrator->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator",
		 "'" + data->get_filename() + "' does not match "
		 "'" + calibrator->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = data->get_Integration (isub);
    MJD epoch = integration->get_epoch ();

    if (epoch < start_epoch)
      start_epoch = epoch;
    if (epoch > end_epoch)
      end_epoch = epoch;

    model[0]->parallactic.set_epoch (epoch);
    float PA = model[0]->parallactic.get_param (0);

    if (PA < PA_min)
      PA_min = PA;
    if (PA > PA_max)
      PA_max = PA;

    // the noise power in the baseline is used to estimate the
    // variance in each Stokes parameter
    vector< vector< double > > baseline_variance;
    integration->baseline_stats (0, &baseline_variance);

    for (unsigned ichan=0; ichan<nchan; ichan++) try {

      if (integration->get_weight (ichan) == 0) {
	cerr << "Pulsar::ReceptionCalibrator::add_observation ichan="
	     << ichan << " flagged invalid" << endl;
	continue;
      }

      // the selected pulse phase bins
      Calibration::Argument::Value* arg = model[ichan]->time.new_Value(epoch);

      unsigned xform_index = model[ichan]->Pulsar_path;
      Calibration::CoherencyMeasurementSet measurements (xform_index);
      measurements.add_coordinate( arg );

      for (unsigned istate=0; istate < pulsar.size(); istate++) {

	Stokes<float> variance;
	for (unsigned ipol=0; ipol < variance.size(); ipol++)
	  variance[ipol] = baseline_variance[ipol][ichan];

	add_data (measurements, pulsar[istate], ichan, integration, variance);

      }

      model[ichan]->equation->add_data (measurements);

    }
    catch (Error& error) {
      cerr << "Pulsar::ReceptionCalibrator::add_observation ichan="
	   << ichan << " error" << error.get_message() << endl;
    }

  }
}

void
Pulsar::ReceptionCalibrator::add_data
( vector<Calibration::CoherencyMeasurement>& bins,
  SourceEstimate& estimate,
  unsigned ichan,
  const Integration* data,
  Stokes<float>& variance
  )
{
  unsigned nchan = data->get_nchan ();

  // sanity check
  if (estimate.source.size () != nchan)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::add_data",
		 "SourceEstimate.nchan=%d != Integration.nchan=%d",
		 estimate.source.size(), nchan);

  unsigned ibin = estimate.phase_bin;

  Stokes<float> value = data->get_Stokes ( ichan, ibin );

  Stokes< Estimate<double> > stokes;

  for (unsigned ipol=0; ipol<stokes.size(); ipol++) {

    stokes[ipol].val = value[ipol];
    stokes[ipol].var = variance[ipol];

  }

  try {

    if (normalize_by_invariant) 
      normalizer.normalize (stokes);

    // NOTE: the measured states are not corrected
    Calibration::CoherencyMeasurement state (estimate.input_index);
    state.set_stokes( stokes );
    bins.push_back ( state );

    /* Correct the stokes parameters using the current best estimate of
       the instrument and the parallactic angle rotation before adding
       them to best estimate of the input state */
    
    Jones< Estimate<double> > correct;
    correct = inv( model[ichan]->pulsar_path->evaluate() );
    
    stokes = transform( stokes, correct );
    
    estimate.source_guess[ichan].integrate( stokes );

  }
  catch (Error& error) {
    cerr << "Pulsar::ReceptionCalibrator::add_data ichan=" << ichan 
	 << " ibin=" << ibin << " error\n\t" << error.get_message() << endl;
  }
}

//! Add the ReferenceCalibrator observation to the set of constraints
void 
Pulsar::ReceptionCalibrator::add_calibrator (const ReferenceCalibrator* p)
try {

  check_ready ("Pulsar::ReceptionCalibrator::add_calibrator");

  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::add_calibrator" << endl;

  const Archive* cal = p->get_Archive();

  if (cal->get_state() != Signal::Coherence)
    throw Error (InvalidParam, 
		 "Pulsar::ReceptionCalibrator::add_calibrator",
		 "Archive='" + cal->get_filename() + "' "
		 "invalid state=" + State2string(cal->get_state()));

  if ( cal->get_type() != Signal::FluxCalOn && 
       cal->get_type() != Signal::PolnCal )
    throw Error (InvalidParam,
                 "Pulsar::ReceptionCalibrator::add_calibrator",
                 "invalid source=" + Source2string(cal->get_type()));

  string reason;
  if (!calibrator->calibrator_match (cal, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_observation",
		 "mismatch between calibrators\n\t" 
		 + calibrator->get_filename() +
                 " and\n\t" + cal->get_filename() + reason);

  unsigned nchan = calibrator->get_nchan ();
  unsigned nsub = cal->get_nsubint();
  unsigned npol = cal->get_npol();
  
  assert (npol == 4);

  if (calibrator_estimate.source.size() == 0) {

    cerr << "Pulsar::ReceptionCalibrator::add_calibrator first cal" << endl;

    // add the calibrator states to the equations
    init_estimate (calibrator_estimate);

    // set the initial guess and fit flags
    Stokes<double> cal_state (1,0,.5,0);

    for (unsigned ichan=0; ichan<nchan; ichan++) {
      
      calibrator_estimate.source[ichan].set_stokes ( cal_state );

      for (unsigned istokes=0; istokes<4; istokes++)
	calibrator_estimate.source[ichan].set_infit (istokes, false);

      // Stokes U may vary
      calibrator_estimate.source[ichan].set_infit (2, true);

      if (measure_cal_Q)
	// Stokes Q of the calibrator may vary!
	calibrator_estimate.source[ichan].set_infit (1, true);


    }

  }

  bool flux_calibrator = cal->get_type() == Signal::FluxCalOn;

  if (flux_calibrator)
    cerr << "Pulsar::ReceptionCalibrator::add_calibrator FluxCalOn" << endl;

  if (flux_calibrator && flux_calibrator_estimate.source.size() == 0) { 

    // add the flux calibrator states to the equations
    init_estimate (flux_calibrator_estimate);

    // set the initial guess and fit flags
    Stokes<double> flux_cal_state (1,0,0,0);

    for (unsigned ichan=0; ichan<nchan; ichan++) {
      
      flux_calibrator_estimate.source[ichan].set_stokes ( flux_cal_state );

      if (measure_cal_V) {
	// Stokes V of Hydra may not vary
	flux_calibrator_estimate.source[ichan].set_infit (3, false);
	
	// Stokes V of the calibrator may vary!
	calibrator_estimate.source[ichan].set_infit (3, true);
      }

      // Flux Calibrator observations are made through a different backend
      model[ichan]->add_fluxcal_backend();

    }

  }


  const PolarCalibrator* polcal = 0;
  if (model_type == Calibrator::Hamaker)
    polcal = dynamic_cast<const PolarCalibrator*>(p);

  const SingleAxisCalibrator* sacal = 0;
  if (model_type == Calibrator::Britton)
    sacal = dynamic_cast<const SingleAxisCalibrator*>(p);

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  for (unsigned isub=0; isub<nsub; isub++) {

    const Integration* integration = cal->get_Integration (isub);

    ReferenceCalibrator::get_levels (integration, nchan, cal_hi, cal_lo);

    MJD epoch = integration->get_epoch ();

    if (epoch < start_epoch)
      start_epoch = epoch;
    if (epoch > end_epoch)
      end_epoch = epoch;

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      unsigned ipol = 0;

      // transpose [ipol][ichan] output of ReferenceCalibrator::get_levels
      vector< Estimate<double> > cal (npol);
      vector< Estimate<double> > fcal (npol);

      for (ipol = 0; ipol<npol; ipol++) {
	cal[ipol] = cal_hi[ipol][ichan] - cal_lo[ipol][ichan];
	fcal[ipol] = cal_lo[ipol][ichan];
      }

      // convert to Stokes parameters
      Stokes< Estimate<double> > cal_stokes = coherency( convert (cal) );
      cal_stokes *= 2.0;

      Stokes< Estimate<double> > fcal_stokes = coherency( convert (fcal) );
      fcal_stokes *= 2.0;

      try {

	Calibration::Argument::Value* arg = model[ichan]->time.new_Value(epoch);
	Calibration::CoherencyMeasurementSet measurements;
	measurements.add_coordinate( arg );

        // convert to CoherencyMeasurement format
        Calibration::CoherencyMeasurement 
	  state (calibrator_estimate.input_index);

	state.set_stokes( cal_stokes );
        measurements.push_back (state);

	if (flux_calibrator) {
	  // add the flux calibrator
	  Calibration::CoherencyMeasurement fstate 
	    (flux_calibrator_estimate.input_index);
	  fstate.set_stokes( fcal_stokes );

	  measurements.push_back (fstate);
	  measurements.set_transformation_index
	    ( model[ichan]->FluxCalibrator_path );
	}
	else {

          if (!model[ichan]->ReferenceCalibrator_path)
            // Flux Calibrator observations are made through a different backend
            model[ichan]->add_polncal_backend();

	  measurements.set_transformation_index
	    ( model[ichan]->ReferenceCalibrator_path );

        }

        model[ichan]->equation->add_data (measurements);

      }
      catch (Error& error) {
        cerr << "Pulsar::ReceptionCalibrator::add_calibrator ichan="
             << ichan << " error\n" << error.get_message() << endl;
      }

      Jones< Estimate<double> > correct;
      correct = p->get_response(ichan);

      if (flux_calibrator) {

	fcal_stokes = transform( fcal_stokes, correct );
	flux_calibrator_estimate.source_guess[ichan].integrate (fcal_stokes);

      }

      cal_stokes = transform( cal_stokes, correct );
      calibrator_estimate.source_guess[ichan].integrate (cal_stokes);

    }
  }

  if (polcal && polcal->get_transformation_nchan() == nchan)  {

    cerr << "Pulsar::ReceptionCalibrator::add_calibrator add Polar Model" 
	 << endl;

    assert (model.size() == nchan);

    const Calibration::Polar* polar;

    for (unsigned ichan = 0; ichan<nchan; ichan++) {

      if (!polcal->get_transformation_valid (ichan))
        continue;

      polar = dynamic_cast<const Calibration::Polar*>
	( polcal->get_transformation(ichan) );

      if (polar)
	model[ichan]->polar_estimate.integrate( polar );

    }

  }

  if (sacal && sacal->get_transformation_nchan() == nchan)  {
    cerr << "Pulsar::ReceptionCalibrator::add_calibrator add SingleAxis Model"
	 << endl;

    assert (model.size() == nchan);

    const Calibration::SingleAxis* sa;

    for (unsigned ichan = 0; ichan<nchan; ichan++) {

      if (!sacal->get_transformation_valid (ichan))
        continue;

      sa = dynamic_cast<const Calibration::SingleAxis*>
	( sacal->get_transformation(ichan) );

      if (sa) {
	if (flux_calibrator)
	  model[ichan]->fluxcal_backend_estimate.integrate( sa );
	else
	  model[ichan]->physical_estimate.integrate( sa );
      }

    }

  }


}
catch (Error& error) {
  throw error += "Pulsar::ReceptionCalibrator::add_calibrator";
}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::precalibrate (Archive* data)
{
  if (verbose)
    cerr << "Pulsar::ReceptionCalibrator::precalibrate" << endl;

  string reason;
  if (!calibrator->calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_observation",
		 "mismatch between calibrator\n\t" 
		 + calibrator->get_filename() +
                 " and\n\t" + data->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  assert (nchan == model.size());

  vector< Jones<float> > response (nchan);

  bool parallactic_corrected = false;

  for (unsigned isub=0; isub<nsub; isub++) {

    Integration* integration = data->get_Integration (isub);

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      if (!model[ichan]->valid) {

	if (verbose)
	  cerr << "Pulsar::ReceptionCalibrator::precalibrate ichan=" << ichan 
	       << " zero weight" << endl;

	integration->set_weight (ichan, 0.0);

	response[ichan] = Jones<double>::identity();
	continue;

      }

      Calibration::Complex2* signal_path = 0;
      Calibration::ReceptionModel* equation = model[ichan]->equation;

      switch ( data->get_type() )  {
      case Signal::Pulsar:
	signal_path = model[ichan]->pulsar_path;
	parallactic_corrected = true;
	break;
      case Signal::PolnCal:
	signal_path = model[ichan]->pcal_path;
	break;
      case Signal::FluxCalOn:
        equation->set_transformation_index (model[ichan]->FluxCalibrator_path);
        signal_path = equation->get_transformation ();
        break;
      default:
	throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::precalibrate",
		     "unknown Archive type for " + data->get_filename() );
      }

      response[ichan] = Jones<float>::identity();

      if (!signal_path) {
        integration->set_weight (ichan, 0.0);
        continue;
      }

      try {
	model[ichan]->time.set_value( integration->get_epoch() );
	response[ichan] = signal_path->evaluate();
      }
      catch (Error& error) {
        cerr << "Pulsar::ReceptionCalibrator::precalibrate ichan=" << ichan
             << endl << error.warning () << endl;
        integration->set_weight (ichan, 0.0);
        response[ichan] = Jones<float>::identity();
	continue;
      }

      if ( norm(det( response[ichan] )) < 1e-9 ) {
        if (verbose)
          cerr << "Pulsar::ReceptionCalibrator::precalibrate ichan=" << ichan
               << " faulty response" << endl;
        integration->set_weight (ichan, 0.0);
        response[ichan] = Jones<float>::identity();
	continue;
      }
      else
	response[ichan] = inv( response[ichan] );

    }

    Calibrator::calibrate (integration, response);

    
  }

  data->set_poln_calibrated (true);
  data->set_scale (Signal::ReferenceFluxDensity);

  Receiver* receiver = data->get<Receiver>();

  if (!receiver) {
    cerr << "Pulsar::ReceptionCalibrator::precalibrate WARNING: "
      "cannot record corrections" << endl;
    return;
  }

  receiver->set_platform_corrected (parallactic_corrected);
  receiver->set_feed_corrected (true);

}

//! Calibrate the polarization of the given archive
void Pulsar::ReceptionCalibrator::calculate_transformation ()
{
  unsigned nchan = model.size();

  transformation.resize( nchan );

  for (unsigned ichan=0; ichan<nchan; ichan++)  {

    transformation[ichan] = 0;

    if (model[ichan]->valid)  {
      if (model[ichan]->polar)
        transformation[ichan] = model[ichan]->polar;
      else if (model[ichan]->physical)
        transformation[ichan] = model[ichan]->physical;
    }

  }
}

bool Pulsar::ReceptionCalibrator::get_solved () const
{
  return is_fit;
}

void Pulsar::ReceptionCalibrator::solve (int only_ichan)
{
  if (!is_initialized)
    check_ready ("Pulsar::ReceptionCalibrator::solve");

  if (calibrator_estimate.source.size() == 0)
    throw Error (InvalidState, "Pulsar::ReceptionCalibrator::solve",
		 "Without an ReferenceCalibrator observation, "
		 "there remains a degeneracy along the Stokes V axis");

  initialize ();

  unsigned nchan = model.size();

  unsigned start_chan = 0;

  if (only_ichan >= 0)
    start_chan = only_ichan;
#if 0
  else
    cerr << "Pulsar::ReceptionCalibrator::solve CPSR-II aliasing issue:\n"
          "WARNING not solving the first " << start_chan << " channels" <<endl;
#endif

  for (unsigned ichan=start_chan; ichan<nchan; ichan++) try {

    cerr << "Pulsar::ReceptionCalibrator::solve ichan=" << ichan;
    if (!model[ichan]->valid) {
      cerr << " flagged invalid" << endl;
      continue;
    }
    cerr << endl;

    model[ichan]->equation->solve ();

    if (only_ichan >= 0)
      break;

  }
  catch (Error& error) {
    cerr << "Pulsar::ReceptionCalibrator::solve failure ichan=" << ichan
         << endl << error.warning() << endl;
    model[ichan]->valid = false;
  }

  Calibration::ReceptionModel::solve_wait ();

  is_fit = true;
}

void Pulsar::ReceptionCalibrator::initialize ()
{
  if (is_initialized)
    return;

  PA_min *= 180.0/M_PI;
  PA_max *= 180.0/M_PI;

  cerr << "Pulsar::ReceptionCalibrator::solve information:\n"
    "  Parallactic angle ranges from " << PA_min <<
    " to " << PA_max << " degrees" << endl;

  calibrator_estimate.update_source();
  flux_calibrator_estimate.update_source();
 
  for (unsigned istate=0; istate<pulsar.size(); istate++)
    pulsar[istate].update_source ();

  MJD epoch = 0.5 * (end_epoch + start_epoch);
  cerr << "Pulsar::ReceptionCalibrator::solve reference epoch: " << epoch << endl;

  for (unsigned ichan=0; ichan<model.size(); ichan++) {
    model[ichan]->convert.set_reference_epoch ( epoch );
    model[ichan]->update ();
  }

  is_initialized = true;
}

void Pulsar::ReceptionCalibrator::check_ready (const char* method, bool unc)
{
  if (is_fit)
    throw Error (InvalidState, method,
		 "Model has been fit. Cannot add data.");

  if (is_initialized)
    throw Error (InvalidState, method,
		 "Model has been initialized. Cannot add data.");

  if (unc && !calibrator)
    throw Error (InvalidState, method,
		 "Initial observation required.");
}

/*! Mask invalid SourceEstimate states */
void Pulsar::ReceptionCalibrator::valid_mask (const Pulsar::SourceEstimate& src)
{
  if (src.valid.size () != model.size())
    throw Error (InvalidParam, "Pulsar::ReceptionCalibrator::valid_mask",
                 "Pulsar::SourceEstimate::valid.size=%d != model.size=%d",
                 src.valid.size (), model.size());

  for (unsigned ichan=0; ichan < model.size(); ichan++)
    model[ichan]->valid &= src.valid[ichan];
}


/*! Update the best guess of each unknown input state */
void Pulsar::SourceEstimate::update_source ()
{
  valid.resize( source.size() );

  unsigned ichan=0;
  for (ichan=0; ichan < source.size(); ichan++)
    valid[ichan] = true;

  for (ichan=0; ichan < source.size(); ichan++) try {
    source_guess[ichan].update( &(source[ichan]) );
  }
  catch (Error& error) {
    cerr << "Pulsar::SourceEstimate::update_source error ichan=" << ichan
         << endl << error.warning() << endl;
    valid[ichan] = false;
  }
}
