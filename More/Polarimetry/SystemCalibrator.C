/***************************************************************************
 *
 *   Copyright (C) 2008-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/BackendCorrection.h"
#include "Pulsar/BasisCorrection.h"

#include "Pulsar/VariableProjectionCorrection.h"
#include "Pulsar/VariableFaradayRotation.h"
#include "Pulsar/ConfigurableProjection.h"
#include "Pulsar/AuxColdPlasma.h"
#include "Pulsar/BirefringenceHistory.h"

#include "Pulsar/VariableBackendEstimate.h"
#include "Pulsar/SystemCalibratorStepFinder.h"

#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/ConfigurableProjectionExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/InstrumentInfo.h"

#include "Pulsar/ModelParametersReport.h"
#include "Pulsar/InputDataReport.h"
#include "Pulsar/DataAndModelReport.h"
#include "Pulsar/CovarianceReport.h"

#include "Pulsar/ArchiveMatch.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Receiver.h"

#include "MEAL/Complex2Constant.h"
#include "MEAL/CongruenceTransformation.h"

#include "BatchQueue.h"
#include "Pauli.h"

#include "strutil.h"
// #define _DEBUG 1
#include "debug.h"

#include <assert.h>

using namespace std;
using namespace Pulsar;
using namespace Calibration;

/*! 
  If a Archive is provided, and if it contains a
  SystemCalibratorExtension, then the constructed instance can be
  used to calibrate other Archive instances.
*/
SystemCalibrator::SystemCalibrator (Archive* archive)
{
  normalize_by_invariant = false;

  solve_in_reverse_channel_order = false;

  refcal_through_frontend = true;

  guess_physical_calibrator_stokes = false;

  data_submitted = false;
    
  is_prepared = false;
  is_solved = false;
  has_pulsar = false;

  retry_chisq = 0.0;
  invalid_chisq = 0.0;

  get_data_fail = 0;
  get_data_call = 0;

  cal_outlier_threshold = 0.0;
  cal_intensity_threshold = 1.0;    // sigma
  cal_polarization_threshold = 0.5; // fraction of I

  match_check_nchan = true;
  
  projection = new VariableProjectionCorrection;
  faraday_rotation = new VariableFaradayRotation;

  step_after_cal = false;

  if (archive)
    set_calibrator (archive);
}

void SystemCalibrator::share (SystemCalibrator* other)
{
  partner = other;
}

void SystemCalibrator::setup_sharing (unsigned ichan)
{
  if (verbose > 1)
    cerr << "SystemCalibrator::setup_sharing ichan=" << ichan << endl;
  
  if (!partner)
    throw Error (InvalidParam, "SystemCalibrator::set_sharing",
		 "no sharing partner");

  if (model.size() == 0)
    throw Error (InvalidState, "SystemCalibrator::setup_sharing",
		 "model not created");

  if (partner->model.size() != this->model.size())
    throw Error (InvalidState, "SystemCalibrator::setup_sharing",
		 "partner nchan=%u != this nchan=%u",
		 partner->model.size(), this->model.size());

  // TO DO other checks, like nchan, freq, bw, etc.
  model[ichan]->share( partner->model[ichan] );
}

void SystemCalibrator::set_calibrator (const Archive* archive)
{
  if (!archive)
    return;

  PolnCalibrator::set_calibrator(archive);

  extension = archive->get<const PolnCalibratorExtension>();
  calibrator_stokes = archive->get<const CalibratorStokes>();

  instance_name = archive->get_source ();
}

void SystemCalibrator::set_name (const std::string& name)
{
  instance_name = name;
}

std::string SystemCalibrator::get_name () const
{
  return instance_name;
}

void SystemCalibrator::set_projection (VariableTransformationManager* _projection)
{
  projection = _projection;
}

void SystemCalibrator::set_faraday_rotation (VariableFaradayRotation* rot)
{
  faraday_rotation = rot;
}

void SystemCalibrator::set_normalize_by_invariant (bool flag)
{
  normalize_by_invariant = flag;
}

void SystemCalibrator::set_normalize_calibrated_by_invariant (bool flag)
{
  normalize_calibrated_by_invariant = flag;
}

//! Return true if least squares minimization solvers are available
bool SystemCalibrator::has_solver () const
{
  return true;
}

//! Return the transformation for the specified channel
const Calibration::ReceptionModel::Solver* 
SystemCalibrator::get_solver (unsigned ichan) const
{
  check_ichan ("get_solver", ichan);
  return model[ichan]->get_equation()->get_solver();
}

//! Return the transformation for the specified channel
Calibration::ReceptionModel::Solver* 
SystemCalibrator::get_solver (unsigned ichan)
{
  assert (ichan < model.size());
  return model[ichan]->get_equation()->get_solver();
}

void SystemCalibrator::set_solver (Calibration::ReceptionModel::Solver* s)
{
  DEBUG("SystemCalibrator::set_solver this=" << this << " ptr=" << (void*) s);
  solver = s;
}

Calibration::ReceptionModel::Solver* SystemCalibrator::get_solver ()
{
  if (!solver)
    solver = Calibration::ReceptionModel::new_default_Solver ();

  DEBUG("SystemCalibrator::get_solver this=" << this << " ptr=" << (void*) solver);
  return solver;
}

//! Copy constructor
SystemCalibrator::SystemCalibrator (const SystemCalibrator& calibrator)
{
}

//! Destructor
SystemCalibrator::~SystemCalibrator ()
{
}

//! Set the algorithm used to automatically insert steps in response
void SystemCalibrator::set_step_finder (StepFinder* finder)
{
  step_finder = finder;
}

//! Get the algorithm used to automatically insert steps in response
SystemCalibrator::StepFinder* SystemCalibrator::get_step_finder ()
{
  return step_finder;
}

Calibrator::Info* SystemCalibrator::get_Info () const
{
  export_prepare ();

  return PolnCalibrator::get_Info ();
}

void SystemCalibrator::set_refcal_through_frontend (bool flag)
{
  refcal_through_frontend = flag;
}

MJD SystemCalibrator::get_epoch () const
{
  return 0.5 * (start_epoch + end_epoch);
}

void SystemCalibrator::add_epoch (const MJD& epoch)
{
  if (epoch < start_epoch || start_epoch == MJD::zero)
    start_epoch = epoch;
  if (epoch > end_epoch || end_epoch == MJD::zero)
    end_epoch = epoch;
}

//! Get the total number of input polarization states
unsigned SystemCalibrator::get_nstate () const
{
  unsigned nstate = 0;

  for (unsigned i=0; i<model.size(); i++)
    nstate = std::max (nstate, model[i]->get_equation()->get_num_input ());

  return nstate;
}

//! Return true if the state index is a pulsar
unsigned SystemCalibrator::get_state_is_pulsar (unsigned istate) const
{
  for (unsigned i=0; i < calibrator_estimate.size(); i++)
    if (calibrator_estimate[i])
      return istate != calibrator_estimate[i]->input_index;

  return true;
}

//! Get the number of pulsar polarization states in the model
unsigned SystemCalibrator::get_nstate_pulsar () const
{
  return 0;
}
    
//! Retern a new plot information interface for the specified pulsar state
Calibrator::Info* 
SystemCalibrator::new_info_pulsar (unsigned istate) const
{
  throw Error (InvalidState, "SystemCalibrator::new_info_pulsar",
	       "not implemented");
}

unsigned SystemCalibrator::get_nchan () const
{
  unsigned nchan = 0;

  if (has_calibrator())
  {
    nchan = get_calibrator()->get_nchan ();
    if (verbose > 2)
      cerr << "SystemCalibrator::get_nchan using calibrator nchan=" << nchan << endl;
  }

  if (model.size())
  {
    nchan = model.size ();
    if (verbose > 2)
      cerr << "SystemCalibrator::get_nchan using model nchan=" << nchan << endl;
  }

  if (verbose > 2)
    cerr << "SystemCalibrator::get_nchan " << nchan << endl;

  return nchan;
}

unsigned SystemCalibrator::get_ndata (unsigned ichan) const
{
  check_ichan ("get_ndata", ichan);
  return model[ichan]->get_equation()->get_ndata ();
}

using namespace MEAL;

void SystemCalibrator::set_response( MEAL::Complex2* f )
{
  response = f;
  type = new_CalibratorType (response);

  if (verbose)
    cerr << "SystemCalibrator::set_response name="
	 << response->get_name() << " type=" << type->get_name() << endl;
}

void SystemCalibrator::set_response_variation( unsigned iparam,
                                                       Univariate<Scalar>* f )
{
  response_variation[iparam] = f;
}

void SystemCalibrator::set_impurity( MEAL::Real4* f )
{
  impurity = f;
}

void SystemCalibrator::set_foreach_calibrator (const MEAL::Complex2* x)
{
  foreach_calibrator = x;
}

void SystemCalibrator::set_stepeach_calibrator (const Calibration::VariableBackend* x)
{
  stepeach_calibrator = x;
}

void SystemCalibrator::set_gain( Univariate<Scalar>* f )
{
  gain_variation = f;
}

void SystemCalibrator::set_diff_gain( Univariate<Scalar>* f )
{
  diff_gain_variation = f;
}

void SystemCalibrator::set_diff_phase( Univariate<Scalar>* f )
{
  diff_phase_variation = f;
}

void SystemCalibrator::add_gain_step (const MJD& mjd)
{
  gain_steps.push_back (mjd);
}

void SystemCalibrator::add_diff_gain_step (const MJD& mjd)
{
  diff_gain_steps.push_back (mjd);
}

void SystemCalibrator::add_diff_phase_step (const MJD& mjd)
{
  diff_phase_steps.push_back (mjd);
}

//! Add a VariableBackend step at the specified MJD
void SystemCalibrator::add_step (const MJD& mjd, Calibration::VariableBackend* backend)
{
  if (model.size() == 0)
    throw Error (InvalidState, "SystemCalibrator::add_step",
		 "model not initialized");

  for (unsigned i=0; i<model.size(); i++)
    model[i]->add_step (mjd, backend->clone());
}

void SystemCalibrator::preprocess (Archive* data)
{
  if (!data)
    return;

  if (data->get_type() == Signal::Pulsar)
  {
    if (!has_Receiver())
      set_Receiver(data);
  }

  BackendCorrection correct_backend;

  if( correct_backend.required (data) )
  {
    if (verbose)
      cerr << "SystemCalibrator::preprocess"
              " correct backend" << endl;
    correct_backend (data);
  }
  else if (verbose)
    cerr << "SystemCalibrator::preprocess"
            " backend correction not required" << endl;
}

//! Add the observation to the set of constraints
void SystemCalibrator::add_observation (const Archive* data) try
{
  if (!data)
    return;
  
  if (data->get_type() == Signal::Pulsar)
    add_pulsar (data);
  else
    add_calibrator (data);
}
catch (Error& error)
{
  throw error += "SystemCalibrator::add_observation";
}

//! Add the specified pulsar observation to the set of constraints
void SystemCalibrator::add_pulsar (const Archive* data) try
{
  if (verbose)
    cerr << "SystemCalibrator::add_pulsar data->nchan=" << data->get_nchan() << endl;

  if (!has_Receiver())
    set_Receiver (data);

  prepare (data);

  unsigned nsub = data->get_nsubint ();

  get_data_fail = 0;
  get_data_call = 0;

  for (unsigned isub=0; isub<nsub; isub++)
    add_pulsar( data, isub );

  if (get_data_fail)
    cerr << "\t" << get_data_fail << " failures in " << get_data_call << " data points" << endl;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::add_pulsar";
}

void SystemCalibrator::prepare (const Archive* data) try
{  
  if (verbose)
    cerr << "SystemCalibrator::prepare"
            " filename=" << data->get_filename() << endl;

  match (data);

  if (model.size() == 0)
  {
    if (verbose)
      cerr << "SystemCalibrator::prepare create_model" << endl;

    create_model ();
  }

  has_pulsar = true;

  if (verbose)
    cerr << "SystemCalibrator::prepare load_calibrators" << endl;

  load_calibrators ();
  
  load_previous ();

  if (verbose)
    cerr << "SystemCalibrator::prepare done" << endl;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::prepare (Archive*)";
}

void SystemCalibrator::load_previous() try
{
  if (previous_loaded)
    return;

  unsigned nchan = model.size();

  if (previous && previous->get_nchan() == nchan)
  {
    cerr << "Copying frontend of previous solution" << endl;
    for (unsigned ichan=0; ichan<nchan; ichan++)
      if (previous->get_transformation_valid(ichan))
        model[ichan]->copy_transformation(previous->get_transformation(ichan));
  }

  if (previous_cal && previous_cal->get_nchan() == nchan)
  {
    cerr << "Copying calibrator polarization of previous solution" << endl;
    for (unsigned ichan=0; ichan<model.size(); ichan++)
    {
      if (previous_cal->get_valid(ichan))
      {
        Stokes< Estimate<double> > stokes( previous_cal->get_stokes(ichan) );
        calibrator_estimate[ichan]->source->set_stokes(stokes);
      }
    }
  }

  previous_loaded = true;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::load_previous";
}
  
//! Add the specified pulsar observation to the set of constraints
void SystemCalibrator::add_pulsar (const Archive* data, unsigned isub) try
{
  if (verbose)
    cerr << "SystemCalibrator::add_pulsar data=" << data << " filename=" << data->get_filename() << " isub=" << isub << endl;

  const Integration* integration = data->get_Integration (isub);
  unsigned nchan = integration->get_nchan ();

  if (model.size() != 1 && nchan != model.size())
    throw Error (InvalidState, "SystemCalibrator::add_pulsar",
		 "input data nchan=%d != model nchan=%d", nchan, model.size());

  MJD epoch = integration->get_epoch ();
  add_epoch (epoch);

  if (projection)
  {
    projection->set_archive (data);
    projection->set_subint (isub);

    if (report_projection || verbose)
      cerr << projection->get_description ();
  }

  if (faraday_rotation)
  {
    faraday_rotation->set_archive (data);
    faraday_rotation->set_subint (isub);
    faraday_rotation->set_chan (0);

    if (faraday_rotation->required())
      cerr << faraday_rotation->get_description() << endl;
  }

  Reference::To<Faraday> ism_faraday;
  if ( correct_interstellar_Faraday_rotation &&
       ( data->get_rotation_measure() != 0.0 ) )
  {
    ism_faraday = new Faraday;
    ism_faraday->set_rotation_measure( data->get_rotation_measure() );

    // correct interstellar Faraday rotation wrt centre frequency
    ism_faraday->set_reference_frequency( data->get_centre_frequency() );
  }
  
  // an identifier for this set of data
  string identifier = data->get_filename() + " " + tostring(isub);

  if (verbose)
    cerr << "SystemCalibrator::add_pulsar identifier=" << identifier << endl;

  pulsar_data.push_back ( vector<CoherencyMeasurementSet>() );

  for (unsigned ichan=0; ichan<nchan; ichan++) try
  {
    if (integration->get_weight (ichan) == 0)
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::add_pulsar ichan=" << ichan << " flagged invalid" << endl;
      continue;
    }
    
    unsigned mchan = ichan;
    if (model.size() == 1)
      mchan = 0;

    using MEAL::Argument;
    
    // epoch abscissa
    Argument::Value* time = model[mchan]->time.new_Value( epoch );

    // measurement set
    Calibration::CoherencyMeasurementSet measurements;

    measurements.set_identifier( identifier );
    measurements.add_coordinate( time );
    measurements.set_ichan( ichan );
    measurements.set_epoch( epoch );
    measurements.set_name( data->get_source() );

    if (projection)
    {
      // projection transformation
      projection->set_chan (ichan);
      measurements.add_coordinate( projection->new_value() );
    }

    if (faraday_rotation)
    {
      // Faraday rotation
      faraday_rotation->set_chan (ichan);
      measurements.add_coordinate( faraday_rotation->new_value () );
    }

    try
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::add_pulsar call add_pulsar ichan=" << ichan << endl;
  
      add_pulsar (measurements, integration, ichan);
    }
    catch (Error& error)
    {
      if (verbose > 2 || error.get_code() != InvalidParam)
        cerr << "SystemCalibrator::add_pulsar ichan=" << ichan << " error=" << error << endl;
    }
    catch (std::exception& error)
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::add_pulsar ichan=" << ichan << " exception=" << error.what() << endl;
    }

    pulsar_data.back().push_back (measurements);
  }
  catch (Error& error)
  {
    cerr << "SystemCalibrator::add_pulsar ichan=" << ichan << " error\n" << error << endl;
    if (error.get_code() != InvalidParam)
      throw error += "SystemCalibrator::add_pulsar";
  }

  if (verbose > 2)
    cerr << "SystemCalibrator::add_pulsar subint exit" << endl;
}
catch (Error& error)
{
  cerr << "SystemCalibrator::add_pulsar error\n" << error << endl;
  throw error += "SystemCalibrator::add_pulsar subint";
}

/*! If the data do not match the calibrator 
    - if throw_exception is true, throw an exception 
    - otherwise, return false */
bool SystemCalibrator::match (const Archive* data, bool throw_exception)
{
  if (verbose)
    cerr << "SystemCalibrator::match" << endl;
  
  if (!has_calibrator())
    throw Error (InvalidState, "SystemCalibrator::match",
		 "No Archive containing pulsar data has yet been added");

  Archive::Match match;

  match.set_check_mixable (true);
  match.set_check_nchan (match_check_nchan);

  if (!match.match (get_calibrator(), data))
  {
    mismatch_reason = match.get_reason();

    if (throw_exception)
      throw Error (InvalidParam, "SystemCalibrator::match",
		   "'" + data->get_filename() + "' does not match "
		   "'" + get_calibrator()->get_filename() + match.get_reason());
    else
      return false;
  }

  if (verbose)
    cerr << "SystemCalibrator::match true for data.source=" << data->get_source() 
         << " source=" << get_calibrator()->get_source() << endl;

  return true;
}

void SystemCalibrator::set_calibrators (const vector<string>& n)
{
  calibrator_filenames = n;
}

void SystemCalibrator::set_flux_calibrator (const FluxCalibrator* fluxcal)
{
  flux_calibrator = fluxcal;
}

void SystemCalibrator::set_previous_solution (const PolnCalibrator* polcal)
{
  previous = polcal;
}

void SystemCalibrator::set_previous_cal (const CalibratorStokes* cal)
{
  previous_cal = cal;
}

void SystemCalibrator::set_response_fixed (const std::vector<unsigned>& params)
{
  response_fixed = params;
}

void SystemCalibrator::set_calibrator_preprocessor (Processor* opt)
{
  calibrator_preprocessor = opt;
}

void SystemCalibrator::load_calibrators ()
{
  if (calibrator_filenames.size() == 0)
    return;

  for (unsigned ifile = 0; ifile < calibrator_filenames.size(); ifile++) try
  {
    if (verbose)
      cerr << "SystemCalibrator::load_calibrators loading\n\t"
           << calibrator_filenames[ifile] << endl;

    Reference::To<Archive> archive;
    archive = Archive::load(calibrator_filenames[ifile]);

    if (calibrator_preprocessor)
      calibrator_preprocessor->process (archive);

    preprocess (archive);
    add_calibrator (archive);    
  }
  catch (Error& error)
  {
    cerr << "SystemCalibrator::load_calibrators" << error << endl;
  }

  calibrator_filenames.resize (0);

  unsigned nchan = model.size();

  cerr << "Setting " << nchan << " channel receiver" << endl;

  for (unsigned ichan=0; ichan<nchan; ichan++) try
  {
    if (!model[ichan])
      throw Error (InvalidState, "SystemCalibrator::load_calibrators",
                   "model[%d] == NULL", ichan);

    if (model[ichan]->get_valid())
      model[ichan]->update ();
  }
  catch (Error& error)
  {
    model[ichan]->set_valid( false, "update failed" );
  }

  if (calibrator_estimate.size() == nchan)
  {  
    for (unsigned ichan=0; ichan<nchan; ichan++) try
    {
      if (!model[ichan]->get_valid())
        continue;

      if (!calibrator_estimate[ichan])
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::load_calibrators"
            " no estimate ichan=" << ichan << endl;
        continue;
      }
    
      Estimate<double> I = calibrator_estimate[ichan]->source->get_stokes()[0];
      if (I.get_value() == 0)
      {
        cerr << "SystemCalibrator::load_calibrators"
          " reference flux equals zero \n"
          "\t attempts=" << calibrator_estimate[ichan]->add_data_attempts <<
          "\t failures=" << calibrator_estimate[ichan]->add_data_failures << endl;
        
        throw Error (InvalidState, "SystemCalibrator::load_calibrators",
              "reference flux equals zero");
      }
      
      if (fabs(I.get_value()-1.0) > I.get_error() && verbose)
        cerr << "SystemCalibrator::load_calibrators warning"
          " ichan=" << ichan << " reference flux=" << I << " != 1" << endl;
    }
    catch (Error& error)
    {
      if (model[ichan])
        model[ichan]->set_valid( false, error.get_message().c_str() );
    }
  }
  
  unsigned ok = 0;
  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (model[ichan]->get_valid())
      ok ++;

  if (ok == 0)
    throw Error (InvalidState, "SystemCalibrator::load_calibrators",
                 "zero valid models");
}

//! Add the specified pulsar observation to the set of constraints
void SystemCalibrator::add_calibrator (const Archive* data)
{
  if (!has_pulsar)
  {
    if (verbose)
      cerr << "SystemCalibrator::add_calibrator postponed until pulsar added" << endl;

    calibrator_filenames.push_back( data->get_filename() );

    return;
  }

  if (!has_Receiver())
    set_Receiver (data);

  try
  {
    Reference::To<ReferenceCalibrator> polncal;

    if (type->is_a<CalibratorTypes::van09_Eq>())
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::add_calibrator new PolarCalibrator" << endl;

      polncal = new PolarCalibrator (data);
    }
    else
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::add_calibrator new SingleAxisCalibrator" << endl;

      polncal = new SingleAxisCalibrator (data);
    }

    polncal->set_nchan( get_calibrator()->get_nchan() );
    polncal->set_outlier_threshold( cal_outlier_threshold );
    
    add_calibrator (polncal);
  }
  catch (Error& error)
  {
    throw error += "SystemCalibrator::add_calibrator (Archive*)";
  }
}

//! Add the ReferenceCalibrator observation to the set of constraints
void SystemCalibrator::add_calibrator (const ReferenceCalibrator* p) try 
{
  unsigned nchan = get_nchan ();

  if (verbose > 2)
    cerr << "SystemCalibrator::add_calibrator nchan=" << nchan << endl;

  if (!nchan)
    throw Error (InvalidState, "SystemCalibrator::add_calibrator", "nchan == 0");

  const Archive* cal = p->get_Archive();

  if (cal->get_state() != Signal::Coherence)
    throw Error (InvalidParam, "SystemCalibrator::add_calibrator",
		 "Archive='" + cal->get_filename() + "' "
		 "invalid state=" + State2string(cal->get_state()));

  if ( cal->get_type() != Signal::FluxCalOn && 
       cal->get_type() != Signal::FluxCalOff && 
       cal->get_type() != Signal::PolnCal )
    throw Error (InvalidParam, "SystemCalibrator::add_calibrator",
                 "invalid source=" + Source2string(cal->get_type()));

  string reason;
  if (!calibrator_match (cal, reason))
    throw Error (InvalidParam, "SystemCalibrator::add_calibrator",
		 "mismatch between \n\t" 
		 + get_calibrator()->get_filename() +
                 " and \n\t" + cal->get_filename() + reason);

  unsigned nsub = cal->get_nsubint();
  unsigned npol = cal->get_npol();
  
  assert (npol == 4);

  Signal::Source source = cal->get_type();

  if (model.size() == 0)
    create_model ();

  if (verbose > 2)
    cerr << "SystemCalibrator::add_calibrator prepare calibrator source=" << source << " nsubint=" << nsub << endl;

  prepare_calibrator_estimate( source );

  if (verbose > 2)
    cerr << "SystemCalibrator::add_calibrator prepare calibrator ok" << endl;

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  epoch_added = vector<bool> (nchan, false);

  // ensure that model array is large enough
  check_ichan ("add_calibrator", nchan - 1);

  Reference::To<PolnCalibrator> solution = p->clone();

  if (flux_calibrator)
  {
    /* Combine ReferenceCalibrator with CalibratorStokes estimate from
       FluxCalibrator stokes into a new HybridCalibrator */
    Reference::To<HybridCalibrator> hybrid_cal = new HybridCalibrator;
    hybrid_cal->set_reference_input( flux_calibrator->get_CalibratorStokes(),
				     flux_calibrator->get_filenames() );
        
    hybrid_cal->set_reference_observation( p );

    solution = hybrid_cal;
  }

  vector< Jones<double> > response (nchan, 0.0);
  vector< Reference::To<const MEAL::Complex2> > xform (nchan);

  // save the solution derived from all sub-integrations
  if ( solution->get_nchan() == nchan )
  {
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if ( solution->get_transformation_valid (ichan) )
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::add_calibrator ichan=" << ichan << " saving response" << endl;

        response[ichan] = solution->get_response(ichan);

        if (verbose > 2)
          cerr << "SystemCalibrator::add_calibrator ichan=" << ichan << " saving transformation" << endl;

        xform[ichan] = solution->get_transformation(ichan);
      }
      else if (verbose > 2)
      {
        cerr << "SystemCalibrator::add_calibrator ichan=" << ichan << " transformation not valid" << endl;
      }
    }
  }

  for (unsigned isub=0; isub<nsub; isub++)
  {
    const Integration* integration = cal->get_Integration (isub);

    solution->set_subint (isub);

    MJD epoch = integration->get_epoch();

    if (verbose)
      cerr << "SystemCalibrator::add_calibrator nchan=" << nchan
           << " outlier_threshold=" << cal_outlier_threshold << endl;
    
    ReferenceCalibrator::get_levels (integration, nchan, cal_hi, cal_lo, cal_outlier_threshold);
    
    string identifier = cal->get_filename() + " " + tostring(isub);

    calibrator_data.push_back ( vector<SourceObservation>() );
    
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (integration->get_weight (ichan) == 0 || !model[ichan]->get_valid())
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::add_calibrator ichan=" << ichan << " flagged invalid" << endl;
        continue;
      }

      // transpose [ipol][ichan] output of ReferenceCalibrator::get_levels
      vector< Estimate<double> > calibtor (npol);
      vector< Estimate<double> > baseline (npol);

      for (unsigned ipol = 0; ipol<npol; ipol++)
      {
        calibtor[ipol] = cal_hi[ipol][ichan] - cal_lo[ipol][ichan];
        baseline[ipol] = cal_lo[ipol][ichan];
      }

      SourceObservation data;

      data.name = cal->get_source();
      data.identifier = identifier;
      data.source = source;
      data.epoch = epoch;
      data.ichan = ichan;
      
      // convert to Stokes parameters
      data.observation = coherency( convert (calibtor) );
      data.baseline = coherency( convert (baseline) );

      Estimate<double> calI = data.observation[0];
      if (calI.get_value() < cal_intensity_threshold * calI.get_error())
      {
        if (Archive::verbose > 1)
          cerr << "Pulsar::SystemCalibrator::add_calibrator ichan=" << ichan
               << " signal not detected" << endl;
        continue;
      }

      Estimate<double> calp = data.observation.abs_vect ();
      if (calp.get_value() < cal_polarization_threshold * calI.get_value())
      {
        if (Archive::verbose > 1)
          cerr << "Pulsar::SystemCalibrator::add_calibrator ichan=" << ichan
               << " signal less than " << cal_polarization_threshold 
               << " polarized" << endl;
        continue;
      }

      if ( xform[ichan] )
      {
        data.response = response[ichan];
        data.xform = xform[ichan];
      }

      calibrator_data.back().push_back (data);
    }
  }
}
catch (Error& error)
{
  throw error += "SystemCalibrator::add_calibrator (ReferenceCalibrator)";
}


void SystemCalibrator::submit_calibrator_data () try 
{
  unsigned nsub = calibrator_data.size();
  
  if (verbose > 2)
    cerr << "SystemCalibrator::add_calibrator nsub=" << nsub << endl;

  if (!nsub)
    return;

  for (unsigned isub=0; isub<nsub; isub++)
  {
    unsigned nchan = calibrator_data[isub].size();
    unsigned ichan = 0;

    if (nchan && verbose > 2)
      cerr << "SystemCalibrator::submit_calibrator_data isub="
           << isub << " submit_calibrator_data source="
           << calibrator_data[isub][0].source << endl;

    for (unsigned jchan=0; jchan<nchan; jchan++) try
    {
      SourceObservation& data = calibrator_data[isub][jchan];
      
      ichan = data.ichan;

      if (!calibrator_estimate[ichan])
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::add_calibrator no estimate ichan=" << ichan << endl;
        continue;
      }
      
      CoherencyMeasurementSet measurements;

      calibrator_estimate[ichan]->add_data_attempts ++;

      measurements.set_identifier( data.identifier );
      measurements.add_coordinate( model[ichan]->time.new_Value(data.epoch) );

      // convert to CoherencyMeasurement format
      CoherencyMeasurement state (calibrator_estimate[ichan]->input_index);

      state.set_stokes( data.observation );
      measurements.push_back( state );

      if (verbose > 2)
        cerr << "SystemCalibrator::submit_calibrator_data ichan=" << ichan << " submit_calibrator_data" << endl;
	
      submit_calibrator_data( measurements, data );

      Jones< Estimate<double> > zero (0.0);

      if ( data.response != zero )
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::submit_calibrator_data ichan=" << ichan << " integrate_calibrator_data" << endl;
            
        integrate_calibrator_data( data );
      }
      else if (verbose > 2)
        cerr << "SystemCalibrator::submit_calibrator_data ichan="
            << ichan << " no response; not integrating" << endl;
      
      if ( data.xform )
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::submit_calibrator_data ichan=" << ichan << " integrate_calibrator_solution" << endl;

        integrate_calibrator_solution( data );
      }
    }
    catch (Error& error)
    {
      cerr << "SystemCalibrator::submit_calibrator_data ichan=" << ichan << " error\n" << error << endl;

      if (calibrator_estimate[ichan])
        calibrator_estimate[ichan]->add_data_failures ++;
      
      continue;
    }
  }
}
catch (Error& error) 
{
  throw error += "SystemCalibrator::submit_calibrator_data ()";
}


void SystemCalibrator::submit_pulsar_data () try 
{
  unsigned nsub = pulsar_data.size();
  
  if (verbose > 2)
    cerr << "SystemCalibrator::submit_pulsar_data nsub=" << nsub << endl;

  if (!nsub)
    return;

  for (unsigned isub=0; isub<nsub; isub++)
  {
    unsigned nchan = pulsar_data[isub].size();
    unsigned ichan = 0;

    if (verbose > 2)
      cerr << "SystemCalibrator::submit_pulsar_data isub=" << isub << " nchan=" << nchan << endl;

    if (nchan == 0)
      continue;

    if (nchan && verbose > 2)
      cerr << "SystemCalibrator::submit_pulsar_data isub=" << isub << " name=" << pulsar_data[isub][0].get_name() << endl;

    for (unsigned jchan=0; jchan<nchan; jchan++) try
    {
      CoherencyMeasurementSet& data = pulsar_data[isub][jchan];
      
      ichan = data.get_ichan();
      
      if (verbose > 2)
        cerr << "SystemCalibrator::submit_pulsar_data ichan=" << ichan << " submit_pulsar_data" << endl;

      // add pulsar data constraints to measurement equation
      submit_pulsar_data( data );

      if (verbose > 2)
        cerr << "SystemCalibrator::submit_pulsar_data ichan=" << ichan << " integrate_pulsar_data" << endl;

      // add pulsar data to mean estimate used as initial guess
      integrate_pulsar_data( data );
    }
    catch (Error& error)
    {
      cerr << "SystemCalibrator::submit_pulsar_data ichan="
           << ichan << " error\n"
           << error << endl;
      continue;
    }
  }
}
catch (Error& error) 
{
  throw error += "SystemCalibrator::submit_pulsar_data ()";
}

void SystemCalibrator::submit_pulsar_data
( Calibration::CoherencyMeasurementSet& measurements) try
{
  unsigned ichan = measurements.get_ichan();
  MJD epoch = measurements.get_epoch();

#if _DEBUG
    cerr << "SystemCalibrator::submit_pulsar_data ichan=" << ichan
         << " model.size=" << model.size() << endl;
#endif
    
  VariableBackendEstimate* backend = model[ichan]->get_backend (epoch);
  IndexedProduct* product = backend->get_psr_response ();

  if (!product->has_index())
  {
    DEBUG("SystemCalibrator::submit_pulsar_data call add_psr_path");
    model[ichan]->add_psr_path (backend);
  }
    
  measurements.set_transformation_index (product->get_index ());

#if _DEBUG
  cerr << "SystemCalibrator::submit_pulsar_data source indeces:";
  for (auto m: measurements)
    cerr << " " << m.get_input_index();
  cerr << endl;
#endif

  if (normalize_by_invariant)
  {
    measurements.set_coordinates();
    for (auto& measurement : measurements)
    {
      measurement.set_coordinates();
      auto transformation = get_transformation (epoch, Signal::Pulsar, ichan);
      Jones< Estimate<double> > xform = transformation->evaluate();
      double determinant = std::abs(det(xform)).get_value();
      measurement.scale(determinant);
    }
  }

  DEBUG("SystemCalibrator::submit_pulsar_data chan=" << ichan);
  model[ichan]->get_equation()->add_data (measurements);

  model[ichan]->add_observation_epoch (epoch);
  backend->add_weight (1.0);
}
catch (Error& error)
{
  cerr << "SystemCalibrator::submit_pulsar_data error=" << error << endl;
  throw error += "SystemCalibrator::submit_pulsar_data";
}

void SystemCalibrator::init_estimates
( std::vector< Reference::To<Calibration::SourceEstimate> >& estimate, int ibin ) try
{
  unsigned nchan = get_nchan ();
  unsigned nbin = get_calibrator()->get_nbin ();

  if (verbose > 2)
    cerr << "SystemCalibrator::init_estimates ibin=" << ibin << endl;
    
  if (ibin >= 0 && unsigned(ibin) >= nbin)
    throw Error (InvalidRange, "SystemCalibrator::init_estimate",
                "phase bin=%d >= nbin=%d", ibin, nbin);

  if (verbose > 2)
    cerr << "SystemCalibrator::init_estimate"
            " nchan=" << nchan << " nbin=" << nbin << endl;

  estimate.resize (nchan);

  check_ichan ("init_estimate", nchan - 1);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->get_valid())
      continue;

    estimate[ichan] = new Calibration::SourceEstimate;

    if (verbose > 2)
      cerr << "SystemCalibrator::init_estimates ichan=" << ichan
          << " model=" << (void*) model[ichan]
          << " eq=" << (void*) model[ichan]->get_equation()
          << endl;
    
    estimate[ichan]->create_source( model[ichan]->get_equation() );
    estimate[ichan]->phase_bin = ibin;

    string name_prefix = instance_name;

    if (ibin == Calibration::SourceEstimate::baseline_mean)
    {
      if (verbose < 2)
        cerr << "SystemCalibrator::init_estimates ichan=" << ichan << " assuming baseline mean has zero Stokes V" << endl;
      estimate[ichan]->source->set_infit (3, false);
      name_prefix += "_baseline";
    }
    else
    {
      name_prefix += "_" + tostring(ibin);
    }

    estimate[ichan]->source->set_param_name_prefix( name_prefix );
  }
}
catch (Error& error)
{
  throw error += "SystemCalibrator::init_estimates";
}

void SystemCalibrator::prepare_calibrator_estimate ( Signal::Source s ) try
{
  if (verbose > 2)
    cerr << "SystemCalibrator::prepare_calibrator_estimate partner=" << (void*) partner.ptr() << endl;

  if (calibrator_estimate.size() == 0)
  {
    if (partner)
      copy_calibrator_estimate();
    else
      create_calibrator_estimate();
  }
}
catch (Error& error)
{
  throw error += "SystemCalibrator::prepare_calibrator_estimate";
}

void SystemCalibrator::copy_calibrator_estimate ()
{
  if (!partner)
    throw Error (InvalidParam, "SystemCalibrator::copy_calibrator_estimate",
		 "no sharing partner");

  if (verbose > 2)
    cerr << "SystemCalibrator::copy_calibrator_estimate size="
         << partner->calibrator_estimate.size() << endl;
  
  try
  {
    calibrator_estimate.resize( partner->calibrator_estimate.size() );
    for (unsigned i=0; i<calibrator_estimate.size(); i++)
      if (partner->calibrator_estimate[i])
        calibrator_estimate[i] = partner->calibrator_estimate[i];
  }
  catch (Error& error)
    {
      throw error += "SystemCalibrator::copy_calibrator_estimate";
    }
}

void SystemCalibrator::create_calibrator_estimate () try
{
  if (verbose)
    cerr << "SystemCalibrator::create_calibrator_estimate" << endl;

  // add the calibrator states to the equations
  init_estimates (calibrator_estimate, Calibration::SourceEstimate::reference_source);

  // set the initial guess and fit flags
  Stokes<double> cal_state (1,0,.9,0);

  Signal::Basis basis = get_calibrator()->get_basis ();

  if (basis == Signal::Circular)
    cal_state = Stokes<double> (1,.9,0,0);

  unsigned nchan = get_nchan ();

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (calibrator_estimate[ichan] && calibrator_estimate[ichan]->source)
    {   
      calibrator_estimate[ichan]->source->set_stokes( cal_state );
      calibrator_estimate[ichan]->source->set_infit( 0, false );

      calibrator_estimate[ichan]->source->set_param_name_prefix( "cal" );
    }
}
catch (Error& error)
{
  throw error += "SystemCalibrator::create_calibrator_estimate";
}

void SystemCalibrator::submit_calibrator_data 
(
 CoherencyMeasurementSet& measurements,
 const SourceObservation& data
 )
{
  if (verbose > 2)
    cerr << "SystemCalibrator::submit_calibrator_data ichan="
	 << data.ichan << endl;

  check_ichan ("submit_calibrator_data", data.ichan);

  assert (epoch_added.size() == get_nchan());
  
  if (!epoch_added[data.ichan])
  {
    /*
      SignalPath::add_calibrator_epoch calls add_polncal_backend,
      which constructs the signal path for the reference source.
    */
    model[data.ichan]->add_calibrator_epoch (data.epoch);
    epoch_added[data.ichan] = true;
  }

  VariableBackendEstimate* backend;
  backend = model[data.ichan]->get_backend (data.epoch);
  IndexedProduct* product = backend->get_cal_response ();
  
  if (!product->has_index())
  {
    DEBUG( "SystemCalibrator call add_cal_path" );
    model[data.ichan]->add_cal_path (backend);
  }
    
  measurements.set_transformation_index( product->get_index() );

  DEBUG("SystemCalibrator::submit_calibrator_data ichan=" << data.ichan);
 
  model[data.ichan]->get_equation()->add_data (measurements);

  backend->add_weight (1.0);
}

void SystemCalibrator::integrate_calibrator_data(const SourceObservation& data)
{
  if (!calibrator_estimate.at(data.ichan))
    throw Error (InvalidState, "SystemCalibrator::integrate_calibrator_data",
		 "no calibrator estimate for ichan=%u", data.ichan);
  
  Jones< Estimate<double> > zero (0.0);
  if (data.response == zero)
    throw Error (InvalidState,
                 "SystemCalibrator::integrate_calibrator_data",
                 "Jones matrix equals zero");

  Jones< Estimate<double> > apply = data.response;

  if (refcal_through_frontend)
    apply = invert_basis * apply;

  if (verbose)
    cerr << "SystemCalibrator::integrate_calibrator_data"
            "\n\tinvert_basis=" << invert_basis << 
            "\n\tcorrect=" << data.response << endl;

  Stokes< Estimate<double> > observed = data.observation;
  
  if (guess_physical_calibrator_stokes)
  {
    Estimate<double> p = observed.abs_vect();
    Estimate<double> I = observed[0];
    
    if (p > I)
    {
      cerr << "SystemCalibrator::integrate_calibrator_data ichan=" << data.ichan
           << " correcting over polarization=" << (p-I)/I << endl;

      // 99% polarized is assumed to be close enough to a good guess
      double scale = 0.99 * I.val / p.val;
      for (unsigned i=1; i<4; i++)
        observed[i] *= scale;
    }
  }
  
  Stokes< Estimate<double> > result = transform( observed, apply );

  if (verbose)
    cerr << "SystemCalibrator::integrate_calibrator_data"
            "\n\tapply=" << apply <<
            "\n\tobs=" << observed <<
            "\n\tresult=" << result << endl;

#if 0
  Estimate<double> p = result.abs_vect();
  if (p.val > 1 + p.var)
    cerr << "SystemCalibrator::integrate_calibrator_data ichan=" << data.ichan
         << " overpolarized by more than 1 sigma " << p-1 << endl;
#endif
  
  calibrator_estimate.at(data.ichan)->estimate.integrate (result);
}

void SystemCalibrator::integrate_calibrator_solution(const SourceObservation& data)
{
  if (!model[data.ichan])
    throw Error (InvalidState,
      "SystemCalibrator::integrate_calibrator_solution",
      "model[%u] is null", data.ichan);
  
  if (data.source == Signal::PolnCal) try
  {
    if (verbose > 2)
      cerr << "SystemCalibrator::integrate_calibrator_solution"
              " SignalPath::integrate_calibrator ichan=" << data.ichan << endl;
    
    model[data.ichan]->integrate_calibrator (data.epoch, data.xform);
  }
  catch (Error& error)
  {
    throw error += "SystemCalibrator::integrate_calibrator_solution";
  }
}


const CalibratorStokes*
SystemCalibrator::get_CalibratorStokes () const
{
  if (calibrator_stokes)
    return calibrator_stokes;

  if (verbose > 2) cerr << "SystemCalibrator::get_CalibratorStokes"
		 " create CalibratorStokes Extension" << endl;

  unsigned nchan = get_nchan ();

  if (nchan != calibrator_estimate.size())
    throw Error (InvalidState,
      "SystemCalibrator::get_CalibratorStokes",
      "Calibrator Stokes nchan=%d != Transformation nchan=%d",
      calibrator_estimate.size(), nchan);

  Reference::To<CalibratorStokes> ext = new CalibratorStokes;
    
  ext->set_nchan (nchan);

  if (!refcal_through_frontend)
    ext->set_coupling_point (CalibratorStokes::BeforeIdeal);
  
  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    bool valid = get_transformation_valid(ichan);
      
    ext->set_valid (ichan, valid);
    if (!valid)
      continue;

    if (!calibrator_estimate[ichan])
      continue;
    
    ext->set_stokes (ichan, calibrator_estimate[ichan]->source->get_stokes());
  }
  catch (Error& error)
  {
    cerr << "SystemCalibrator::get_CalibratorStokes ichan="
         << ichan << " error\n" << error.get_message() << endl;
    ext->set_valid (ichan, false);
  }
  
  calibrator_stokes = ext;
  
  return calibrator_stokes;
}

void SystemCalibrator::create_model () try
{
  if (verbose > 2)
    cerr << "SystemCalibrator::create_model" << endl;

#if 0
  // this requirement could be made optional if necessary
  if (!has_Receiver())
    throw Error (InvalidState, "SystemCalibrator::create_model",
                 "receiver not set");
#endif
  
  if (verbose)
    cerr << "SystemCalibrator::create_model receiver basis from"
            "\n\t filename = " << get_receiver_basis_filename() << endl;

  MEAL::Complex2* basis = 0;

  // if no basis correction is required, then default to identity matrix
  invert_basis = 1.0;

  if (has_Receiver())
  {
    if (verbose)
      cerr << "SystemCalibrator::create_model receiver set" << endl;

    Pauli::basis().set_basis( get_Receiver()->get_basis() );

    BasisCorrection basis_correction;

    if (basis_correction.required(get_Receiver()))
    {
      basis = new MEAL::Complex2Constant( basis_correction(get_Receiver()) );

      invert_basis = inv( basis->evaluate() );

      cerr << "SystemCalibrator::create_model basis corrections:\n"
           << basis_correction.get_summary () << endl;

      if (verbose) cerr
          << "SystemCalibrator::create_model receiver=\n  " 
          << basis->evaluate() << endl;
    }
    else if (verbose)
      cerr << "SystemCalibrator::create_model basis correction not required" << endl;
  }
  else if (verbose)
    cerr << "SystemCalibrator::create_model receiver not set" << endl;

  unsigned nchan = get_nchan ();
  model.resize (nchan);

  if (projection)
    projection->set_nchan (nchan);

  if (faraday_rotation)
    faraday_rotation->set_nchan (nchan);

  if (verbose)
    cerr << "SystemCalibrator::create_model nchan=" << nchan << endl;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "SystemCalibrator::create_model ichan=" << ichan
           << " type=" << type->get_name() << endl;

    model[ichan] = new Calibration::SignalPath (type);

    if (basis)
      model[ichan]->set_basis (basis);

    if (partner)
      setup_sharing( ichan );

    init_model( ichan );
  }

  if (report_total_invariant)
  {
    invint_out.open("invint.txt");
    if (!invint_out)
      throw Error (FailedSys, "SystemCalibrator::create_model", "ostream::open(\"invint.txt\")");
  }
  
  if (verbose)
    cerr << "SystemCalibrator::create_model exit" << endl;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::create_model";
}

void SystemCalibrator::init_model (unsigned ichan)
{
  if (verbose > 2)
    cerr << "SystemCalibrator::init_model ichan=" << ichan << endl;

  model[ichan]->set_name ( instance_name + "_" + tostring(ichan) );

  if (!partner)
  {
    if (response_fixed.size())
    {
      if (!response)
        response = Pulsar::new_transformation (type);

      for (unsigned i=0; i < response_fixed.size(); i++)
        response->set_infit ( response_fixed[i], false );
    }
  
    if (response)
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::init_model response name=" << response->get_name() << endl;
      model[ichan]->set_response( response->clone() );
    }

    if (impurity)
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::init_model impurity" << endl;
      model[ichan]->set_impurity( impurity->clone() );
    }
  
    if (projection)
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::init_model projection" << endl;
      model[ichan]->set_projection( projection->get_transformation(ichan) );
    }

    if (faraday_rotation)
    {
      if (verbose > 2)
        cerr << "SystemCalibrator::init_model Faraday rotation" << endl;
      model[ichan]->set_faraday_rotation( faraday_rotation->get_transformation(ichan) );
    }

    for (auto rv : response_variation)
      model[ichan]->set_response_variation( rv.first, rv.second->clone() );

    if (gain_variation)
      model[ichan]->set_gain_variation( gain_variation->clone() );

    if (diff_gain_variation)
      model[ichan]->set_diff_gain_variation( diff_gain_variation->clone() );
  
    if (diff_phase_variation)
      model[ichan]->set_diff_phase_variation( diff_phase_variation->clone() );

    for (unsigned i=0; i < gain_steps.size(); i++)
      model[ichan]->add_gain_step (gain_steps[i]);

    for (unsigned i=0; i < diff_gain_steps.size(); i++)
      model[ichan]->add_diff_gain_step (diff_gain_steps[i]);

    for (unsigned i=0; i < diff_phase_steps.size(); i++)
      model[ichan]->add_diff_phase_step (diff_phase_steps[i]);
  }
  
  model[ichan] -> set_refcal_through_frontend (refcal_through_frontend);

  if (foreach_calibrator)
    model[ichan]->set_foreach_calibrator( foreach_calibrator );

  if (stepeach_calibrator)
    model[ichan]->set_stepeach_calibrator( stepeach_calibrator );

  if (solver)
    model[ichan]->set_solver( solver->clone() );

  Calibration::ReceptionModel* equation = model[ichan]->get_equation();

  string filename;

  if (report_initial_state)
  {
    filename = stringprintf("prefit_model_ichan%04d.txt",ichan);
    equation->add_prefit_report ( new Calibration::ModelParametersReport(filename) );
  }

  if (report_input_data)
  {
    filename = stringprintf("input_data_ichan%04d.txt",ichan);
    equation->add_prefit_report ( new Calibration::InputDataReport(filename) );
  }

  if (report_data_and_model)
  {
    filename = stringprintf("results_init_ichan%04d.dat",ichan);
    equation->add_prefit_report ( new Calibration::DataAndModelReport(filename) );
    
    filename = stringprintf("results_ichan%04d.dat",ichan);
    equation->add_postfit_report ( new Calibration::DataAndModelReport(filename) );
  }

  if (report_covariance)
  {
    filename = stringprintf("covariance_ichan%04d.txt",ichan);
    equation->add_postfit_report ( new Calibration::CovarianceReport(filename) );
  }
}

//! Return the SignalPath for the specified channel
const Calibration::SignalPath*
SystemCalibrator::get_model (unsigned ichan) const
{
  check_ichan ("get_model", ichan);
  return model[ichan];
}

void SystemCalibrator::set_nthread (unsigned nthread)
{
  queue.resize (nthread);
}

void
SystemCalibrator::set_equation_configuration (const vector<string>& c)
{
  equation_configuration = c;
}

template<class Equation>
void print (const Equation* equation, const char* function)
{
  unsigned nparam = equation->get_nparam();
  cerr << function << " nparam=" << nparam << endl;
  for (unsigned i=0; i<nparam; i++)
    cerr << i << ":" <<  equation->get_infit(i) 
         << " name=" << equation->get_param_name(i) << endl;
}

void SystemCalibrator::configure ( MEAL::Function* equation ) try
{
  if (equation_configuration.size() == 0)
    return;

  Reference::To<TextInterface::Parser> interface = equation ->get_interface();
  for (unsigned i=0; i<equation_configuration.size(); i++)
    interface->process (equation_configuration[i]);

  // print (equation, "SystemCalibrator::configure");
}
catch (Error& error)
{
  cerr << "SystemCalibrator::configure " << error << endl;
  print (equation, "SystemCalibrator::configure");
  exit (-1);
}

void SystemCalibrator::close_input_failed ()
{
  for (unsigned ichan=0; ichan < input_failed.size(); ichan++)
  {
    input_failed[ichan]->close();
    delete input_failed[ichan];
  }
  input_failed.resize(0);
}

void SystemCalibrator::print_input_failed 
     (const std::vector<SourceEstimate>& sources)
{
  unsigned nchan = sources.size();

  if (input_failed.size() == 0)
  {
    input_failed.resize( nchan );
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      string filename = "input_failed_" + tostring(ichan) + ".txt";
      input_failed[ichan] = new ofstream (filename.c_str());
    }
  }
  else if (sources.size() != input_failed.size())
    throw Error (InvalidState, "SystemCalibrator::print_input_failed",
                 "source nchan=%u != output nchan=%u",
                 sources.size(), input_failed.size());

  for (unsigned ichan=0; ichan<nchan; ichan++)
    sources[ichan]->report_input_failed (*(input_failed[ichan]));
}

void SystemCalibrator::solve_prepare () try
{
  if (is_prepared)
    return;

  DEBUG("SystemCalibrator::solve_prepare this=" << this);

  if (!data_submitted)
  {
    if (step_finder)
    {
      if (verbose)
        cerr << "SystemCalibrator::prepare finding steps" << endl;

      step_finder->process (this);
    }
    
    if (verbose)
      cerr << "SystemCalibrator::prepare submit_calibrator_data" << endl;

    submit_calibrator_data ();

    if (verbose)
      cerr << "SystemCalibrator::prepare submit_pulsar_data" << endl;

    submit_pulsar_data ();

    data_submitted = true;
  }
    
  if (report_input_failed)
    print_input_failed (calibrator_estimate);
  
  if (!previous_cal)
  {
    for (unsigned ichan=0; ichan<calibrator_estimate.size(); ichan++)
      if (calibrator_estimate[ichan])
      {
        DEBUG("SystemCalibrator::prepare update calibrator estimate ichan=" << ichan);
        calibrator_estimate[ichan]->update ();
      }
  }

  MJD epoch = get_epoch();

  if (verbose)
    cerr << "SystemCalibrator::solve_prepare epoch=" << epoch << endl;

  for (unsigned ichan=0; ichan<model.size(); ichan++)
  {
    if (model[ichan]->get_equation()->get_ndata() == 0)
    {
      if (verbose)
        cerr << "SystemCalibrator::solve_prepare warning"
                " ichan=" << ichan << " has no data" << endl;

      model[ichan]->set_valid( false, "no data" );
    }

    if (!model[ichan]->get_valid())
      continue;

    if (ichan < calibrator_estimate.size() && calibrator_estimate[ichan])
    {
      // sanity check
      Estimate<double> I = calibrator_estimate[ichan]->source->get_stokes()[0];
      if (I.get_value() == 0)
      {
        if (verbose > 1)
          cerr << "SystemCalibrator::solve_prepare"
           " reference flux equals zero \n"
           "\t attempts=" << calibrator_estimate[ichan]->add_data_attempts <<
           "\t failures=" << calibrator_estimate[ichan]->add_data_failures 
               << endl;

        model[ichan]->set_valid( false, "reference flux equals zero" );
      }

      if (fabs(I.get_value()-1.0) > I.get_error() && verbose)
        cerr << "SystemCalibrator::solve_prepare warning ichan=" << ichan << " reference flux=" << I << " != 1" << endl;
    }

    model[ichan]->set_reference_epoch ( epoch );

    model[ichan]->check_constraints ();
    
    model[ichan]->update ();

    configure (model[ichan]->get_equation());

    if (verbose > 2)
      get_solver(ichan)->set_debug();
  }

  if (report_total_invariant)
  {
    invint_out.close();
  }

  if (normalize_calibrated_by_invariant)
  {
    invint_out.open("invint_out.txt");
  }

  is_prepared = true;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::solve_prepare";
}

float SystemCalibrator::get_reduced_chisq (unsigned ichan) const
{
  check_ichan ("get_reduced_chisq", ichan);

  if (!model[ichan]->get_valid ())
    return 0.0;

  const ReceptionModel* equation = model[ichan]->get_equation();

  float chisq = equation->get_solver()->get_chisq ();
  unsigned free = equation->get_solver()->get_nfree ();

  return chisq/free;
}

void SystemCalibrator::solve () try
{
  ReceptionModel::Solver::report_chisq = true;

  if (verbose > 1)
    cerr << "SystemCalibrator::solve calling solve_prepare" << endl;
  
  solve_prepare ();

  unsigned nchan = get_nchan ();

  vector<unsigned> order (nchan);
  if (solve_in_reverse_channel_order)
    for (unsigned ichan=0; ichan<nchan; ichan++)
      order[ichan] = nchan - ichan - 1;
  else
    for (unsigned ichan=0; ichan<nchan; ichan++)
      order[ichan] = ichan;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ order[ichan] ]->get_valid())
    {
      cerr << "channel " << order[ichan] << " flagged invalid"
              " (" << model[ order[ichan] ]->get_invalid_reason() << ")" << endl;
      continue;
    }

    queue.submit( model[ order[ichan] ].get(), &SignalPath::solve );
  }

  queue.wait ();

  unsigned retried = 1;

  while (retried)
  {
    retried = 0;

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      Solver* solver = get_solver( order[ichan] );

      bool to_retry = false;

      if (solver->get_singular())
      {
        cerr << "retry singular channel " << order[ichan] << endl;
        to_retry = true;
      }

      if (solver->get_iterations() >= solver->get_maximum_iterations()) 
      {
        cerr << "retry max iterations channel " << order[ichan] << endl;
        to_retry = true;
      }

      if (to_retry && model[ order[ichan] ]->reduce_nfree())
      {
        resolve (order[ichan]);
        retried ++;
      }
    }

    queue.wait ();
  }

  if (retry_chisq > 0.0)
  { 
    // attempt to fix up any channels that didn't converge well
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ order[ichan] ]->get_valid())
        continue;

      float reduced_chisq = get_reduced_chisq ( order[ichan] );

      if (reduced_chisq > retry_chisq)
      {
        cerr << "try for better fit in ichan=" << order[ichan]
            << " chisq/nfree=" << reduced_chisq << endl;

        resolve ( order[ichan] );
      }
    }

    queue.wait ();
  }

  if (invalid_chisq > 0.0)
  {
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ichan]->get_valid())
        continue;

      float reduced_chisq = get_reduced_chisq (ichan);

      if (reduced_chisq > invalid_chisq)
      {
        cerr << "invalid fit in ichan=" << ichan 
            << " chisq/nfree=" << reduced_chisq << endl;

        model[ichan]->set_valid( false, "bad fit" );
      }
    }
  }

  covariance.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    if (!get_solver(ichan)->get_solved())
      model[ichan]->set_valid( false, "not solved" );

    if (!model[ichan]->get_valid())
      continue;

    model[ichan]->get_covariance( covariance[ichan], get_epoch() );
  }
  catch (Error& error)
  {
    // if (verbose)
      cerr << "SystemCalibrator::solve get_covariance error" << error << endl;
    model[ichan]->set_valid( false, error.get_message().c_str() );
  }

  // ensure that calculate_transformation is called again
  transformation_resize (0);

  is_solved = true;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::solve";
}

void SystemCalibrator::resolve (unsigned ichan) try
{
  unsigned nchan = get_nchan ();

  // look for the nearest neighbour with a solution
  for (int off=1; off < int(nchan); off++)
  {
    for (int dir=-1; dir <= 1; dir+=2)
    {
      int jchan = int(ichan) + dir * off;

      if (jchan < 0 || jchan >= int(nchan))
	continue;

#ifdef _DEBUG
      cerr << "testing " << jchan << " ... ";
#endif

      if (!model[jchan]->get_valid())
      {
#ifdef _DEBUG
        cerr << "not valid" << endl;
#endif
	continue;
      }

      ReceptionModel* equation = model[jchan]->get_equation();

      if (!equation->get_solver()->get_solved())
      {
#ifdef _DEBUG
        cerr << "not solved" << endl;
#endif
	continue;
      }

      float chisq = equation->get_solver()->get_chisq ();
      unsigned free = equation->get_solver()->get_nfree ();
      float reduced_chisq = chisq/free;

      if (retry_chisq > 0.0 && reduced_chisq > retry_chisq)
      {
#ifdef _DEBUG
        cerr << "not good; reduced chisq=" << reduced_chisq << endl;
#endif
	continue;
      }

      cerr << "copying solution from chan=" << jchan 
	   << " chisq/nfree=" << reduced_chisq << endl;

      try {
        model[ichan]->copy( model[jchan] );
      }
      catch (Error& error)
      {
        cerr << "copy failed: " << error.get_message() << endl;
        continue;
      }

      queue.submit( model[ichan].get(), &SignalPath::solve );

      return;
    }
  }

  if (get_solver(ichan)->get_singular())
  {
    queue.submit( model[ichan].get(), &SignalPath::solve );
  }
  else
    cerr << "could not find a suitable solution to copy for retry" << endl;
}
catch (Error& error)
{
  throw error += "SystemCalibrator::resolve";
}

bool SystemCalibrator::get_prepared () const
{
  return is_prepared;
}

bool SystemCalibrator::get_solved () const
{
  return is_solved;
}

bool SystemCalibrator::get_valid (unsigned ichan) const
{
  return model[ichan]->get_valid();
}

void SystemCalibrator::set_valid (unsigned ichan, bool flag, const string& reason)
{
  model[ichan]->set_valid (flag, reason.c_str());
}

bool SystemCalibrator::has_valid () const
{
  unsigned nchan = model.size();
  
  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (model[ichan]->get_valid())
      return true;

  return false;
}

void Pulsar::SystemCalibrator::reset ()
{
  unsigned nchan = model.size();

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::reset" << endl;
  for (unsigned ichan=0; ichan<nchan; ichan++)
    model[ichan]->reset();
}

/*! Retrieves the transformation from the standard model in each channel */
void SystemCalibrator::calculate_transformation ()
{
  unsigned nchan = get_nchan ();

  transformation_resize( nchan );

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    assert (ichan < model.size());

    if (model[ichan]->get_valid())
      set_transformation (ichan, model[ichan]->get_transformation());   
    else
      set_transformation_invalid(ichan, model[ichan]->get_invalid_reason());
  }
}

bool SystemCalibrator::calibrator_match (const Archive* data, std::string& reason)
{
  return get_calibrator()->calibrator_match (data, reason);
}

//! Calibrate the polarization of the given archive
void SystemCalibrator::precalibrate (Archive* data)
{
  if (verbose > 2)
    cerr << "SystemCalibrator::precalibrate" << endl;

  preprocess (data);

  string reason;
  if (!calibrator_match (data, reason))
    throw Error (InvalidParam, "PulsarCalibrator::precalibrate",
        "mismatch between calibrator\n\t" 
        + get_calibrator()->get_filename() +
        " and\n\t" + data->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  if (nchan != model.size() && model.size() != 1)
    throw Error (InvalidState, "SystemCalibrator::precalibrate",
                 "model size=%u != data nchan=%u", model.size(), nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    assert (ichan < model.size());
    model[ichan]->engage_time_variations();
  }

  vector< Jones<float> > response (nchan);

  projection->set_archive (data);
  bool projection_corrected = false;

  if (faraday_rotation)
  {
    faraday_rotation->set_archive (data);
  }

  bool faraday_rotation_corrected = false;

  BackendCorrection correct_backend;
  correct_backend (data);

  bool verbose_model_detail = true;

  for (unsigned isub=0; isub<nsub; isub++)
  {
    Integration* integration = data->get_Integration (isub);

    projection->set_subint (isub);

    if (projection->required ())
      projection_corrected = true;

    if (faraday_rotation)
    {
      faraday_rotation->set_subint (isub);

      if (faraday_rotation->required())
        faraday_rotation_corrected = true;
    }

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      assert (ichan < model.size());

      if (!get_transformation_valid(ichan))
      {
        if (model[ichan]->get_valid())
          cerr << "SystemCalibrator::precalibrate transformation not valid but model valid ichan=" << ichan << endl;

        if (verbose > 2)
          cerr << "SystemCalibrator::precalibrate ichan=" << ichan << " zero weight" << endl;

        integration->set_weight (ichan, 0.0);

        response[ichan] = 0.0;
        continue;
      }

      projection->set_chan (ichan);
      projection->update();

      if (faraday_rotation)
      {
        faraday_rotation->set_chan (ichan);
        faraday_rotation->update();
      }

      try
      {
        auto xform = get_transformation(data, isub, ichan);
        response[ichan] = xform->evaluate();

        if (verbose > 2)
        {
          cerr << "SystemCalibrator::precalibrate chan=" << ichan << " response=" << response[ichan] << endl;

          if (verbose_model_detail)
          {
            string detail;
            xform->print(detail);
            cerr << "SystemCalibrator::precalibrate model detail=\n" << detail << endl;
            verbose_model_detail = false;
          }
        }
      }
      catch (Error& error)
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::precalibrate ichan=" << ichan << endl << error.get_message() << endl;

        integration->set_weight (ichan, 0.0);
        response[ichan] = 0.0;
        continue;
      }

      if (normalize_calibrated_by_invariant)
      {
        double total_invariant = get_invariant(integration, ichan);

        double G = norm(det( response[ichan] ));

        invint_out << integration->get_epoch().printdays(10) << " " << ichan 
                   << " " << total_invariant << " " << sqrt(G) << endl;
        invint_out.flush();

        response[ichan] *= sqrt(total_invariant);
      }

      if ( norm(det( response[ichan] )) < 1e-9 )
      {
        if (verbose > 2)
          cerr << "SystemCalibrator::precalibrate ichan=" << ichan << " faulty response" << endl;

        integration->set_weight (ichan, 0.0);
        response[ichan] = 0.0;
        continue;
      }

      response[ichan] = inv( response[ichan] );
    }

    integration->expert()->transform (response);

    if (faraday_rotation_corrected)
    {
      auto ism = faraday_rotation->get_interstellar_rotation();
      auto iono = faraday_rotation->get_ionospheric_rotation();

      double ism_rm = ism->get_rotation_measure().get_value();
      double iono_rm = iono->get_rotation_measure().get_value();

      if (ism_rm != 0 || iono_rm != 0)
      {
        auto corrected = integration->getadd<BirefringenceHistory>();

        double reference_wavelength = ism->get_reference_wavelength();

        if (ism_rm != 0)
        {
          corrected->get_relative()->set_corrected(true);
          corrected->get_relative()->set_measure(ism_rm);
          corrected->get_relative()->set_reference_wavelength( reference_wavelength );
        }

        if (iono_rm != 0)
        {
          corrected->get_absolute()->set_corrected(true);
          corrected->get_absolute()->set_measure(iono_rm);
          corrected->get_absolute()->set_reference_wavelength( reference_wavelength );
        }
      }
    }
  }

  data->set_poln_calibrated (true);
  data->set_scale (Signal::ReferenceFluxDensity);

  Receiver* receiver = data->get<Receiver>();

  if (!receiver)
  {
    cerr << "SystemCalibrator::precalibrate WARNING: cannot record corrections" << endl;
    return;
  }

  // do not set corrected flag to false if already true
  if ( projection_corrected )
    receiver->set_projection_corrected (true);

  receiver->set_basis_corrected (true);

  if (faraday_rotation_corrected)
  {
    data->set_faraday_corrected (true);
    data->getadd<AuxColdPlasma>()->set_birefringence_corrected(true);
  }
}

MEAL::Complex2* SystemCalibrator::get_transformation (const Archive* data, unsigned isubint, unsigned ichan)
{
  const Integration* integration = data->get_Integration (isubint);
  MJD epoch = integration->get_epoch();

  return get_transformation (epoch, data->get_type(), ichan);
}

MEAL::Complex2* SystemCalibrator::get_transformation (const MJD& epoch, Signal::Source source, unsigned ichan)
{
  VariableBackendEstimate* backend = model[ichan]->get_backend (epoch);

  IndexedProduct* product = 0;
  
  switch ( source )
  {
  case Signal::Pulsar:
    if (verbose > 2)
      cerr << "SystemCalibrator::get_transformation Pulsar" << endl;
    product = backend->get_psr_response ();
    break;

  case Signal::PolnCal:
    if (verbose > 2)
      cerr << "SystemCalibrator::get_transformation PolnCal" << endl;
    product = backend->get_cal_response ();
    break;

  default:
    throw Error (InvalidParam, "SystemCalibrator::get_transformation",
		 "unknown transformation for " + tostring(source) );
  }
  
  ReceptionModel* equation = model[ichan]->get_equation();
  equation->set_transformation_index (product->get_index ());	
  MEAL::Transformation<Complex2>* signal_path = 0;
  signal_path = equation->get_transformation ();
    
  MEAL::CongruenceTransformation* congruence = 0;
  if (signal_path)
    congruence = dynamic_cast<MEAL::CongruenceTransformation*>(signal_path);

  if (!congruence)
    throw Error (InvalidState, "SystemCalibrator::get_transformation",
		 "measurement equation is not a congruence transformation");

  if (verbose > 2)
    cerr << "SystemCalibrator::get_transformation set epoch=" << epoch << endl;
  
  model[ichan]->time.set_value(epoch);
  return congruence->get_transformation();
}

Archive* SystemCalibrator::new_solution (const string& class_name) const try
{
  if (verbose)
    cerr << "SystemCalibrator::new_solution create CalibratorStokes Extension" << endl;

  Reference::To<Archive> output = Calibrator::new_solution (class_name);

  // get rid of the pulsar attributes
  output->set_rotation_measure (0.0);
  output->set_dispersion_measure (0.0);
  output->set_ephemeris (0);
  output->set_model (0);

  if (calibrator_estimate.size())
    output -> add_extension (get_CalibratorStokes()->clone());

  if (projection)
  {
    auto cp = dynamic_cast<ConfigurableProjection*>( projection.get() );
    if (cp)
    {
      if (verbose)
        cerr << "SystemCalibrator::new_solution adding ConfigurableProjectionExtension" << endl;
      output -> add_extension (new ConfigurableProjectionExtension(cp));
    }
  }

  if (has_Receiver())
  {
    Receiver* rcvr = get_Receiver()->clone();

    /*
      WvS - 14 May 2012

      For the PulsarCalibrator (METM) class, the "calibrator" will be
      the well-calibrated pulsar template, which will have had the
      following corrections performed on it.  However, it doesn't make
      sense to store a calibrator solution with these flags set.  In
      fact, before today, the PolnCalibrator would add an incorrectly
      computed basis transformation if the basis_corrected flag was
      set in the calibrator archive, leading to bug #3526460.

      WvS - 26 March 2017
      The PulsarCalibrator class now removes the Receiver extension that
      gets taken from the well-calibrated pulsar template
    */
    
    output -> add_extension (rcvr);
  }

  return output.release();
}
catch (Error& error)
{
  throw error += "SystemCalibrator::new_solution";
}

void SystemCalibrator::set_retry_reduced_chisq (float reduced_chisq)
{
  retry_chisq = reduced_chisq;
}

void SystemCalibrator::set_invalid_reduced_chisq (float reduced_chisq)
{
  invalid_chisq = reduced_chisq;
}

void SystemCalibrator::set_report_projection (bool flag)
{
  report_projection = flag;
}

void SystemCalibrator::set_report_initial_state (bool flag)
{
  report_initial_state = flag;
}

void SystemCalibrator::set_report_input_data (bool flag)
{
  report_input_data = flag;
}

void SystemCalibrator::set_report_data_and_model (bool flag)
{
  report_data_and_model = flag;
}

void SystemCalibrator::set_report_total_invariant (bool flag)
{
  report_total_invariant = flag;
}

void SystemCalibrator::set_report_input_failed (bool flag)
{
  report_input_failed = flag;
}

void SystemCalibrator::set_report_covariance (bool flag)
{
  report_covariance = flag;
}

void SystemCalibrator::check_ichan (const char* name, unsigned ichan) const
{
  if (ichan >= model.size())
    throw Error (InvalidRange, "SystemCalibrator::" + string(name),
		 "ichan=%u >= nchan=%u", ichan, model.size());
}

