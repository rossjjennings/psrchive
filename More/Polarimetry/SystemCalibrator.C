/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/BackendCorrection.h"
#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/PolarCalibrator.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/InstrumentInfo.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Receiver.h"

#include "MEAL/Complex2Constant.h"

#include "BatchQueue.h"
#include "Pauli.h"

#include <assert.h>

using namespace std;
using namespace Calibration;

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  SystemCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::SystemCalibrator::SystemCalibrator (Archive* archive)
{
  set_initial_guess = true;

  is_prepared = false;
  is_solved = false;

  try_again_chisq = 1.5;

  get_data_fail = 0;
  get_data_call = 0;

  if (archive)
    set_calibrator (archive);
}

void Pulsar::SystemCalibrator::set_calibrator (Archive* archive)
{
  if (!archive)
    return;

  PolnCalibrator::set_calibrator(archive);

  extension = archive->get<PolnCalibratorExtension>();
  calibrator_stokes = archive->get<CalibratorStokes>();
}

void 
Pulsar::SystemCalibrator::set_solver (Calibration::ReceptionModel::Solver* s)
{
  solver = s;
}

//! Copy constructor
Pulsar::SystemCalibrator::SystemCalibrator (const SystemCalibrator& calibrator)
{
}

//! Destructor
Pulsar::SystemCalibrator::~SystemCalibrator ()
{
}

Pulsar::SystemCalibrator::Info*
Pulsar::SystemCalibrator::get_Info () const
{
  export_prepare ();

  switch (model_type)
  {   
  case Calibrator::Hamaker:
    return new PolarCalibrator::Info (this);
  case Calibrator::Britton:
    return new InstrumentInfo (this);
  default:
    return 0;
  }
}

Pulsar::Calibrator::Type Pulsar::SystemCalibrator::get_type () const
{
  return model_type;
}

MJD Pulsar::SystemCalibrator::get_epoch () const
{
  return 0.5 * (start_epoch + end_epoch);
}

void Pulsar::SystemCalibrator::add_epoch (const MJD& epoch)
{
  if (epoch < start_epoch || start_epoch == MJD::zero)
    start_epoch = epoch;
  if (epoch > end_epoch || end_epoch == MJD::zero)
    end_epoch = epoch;
}

//! Get the total number of input polarization states
unsigned Pulsar::SystemCalibrator::get_nstate () const
{
  if (model.size() == 0)
    return 0;

  return model[0]->get_equation()->get_num_input ();
}

//! Get the number of pulsar polarization states in the model
unsigned Pulsar::SystemCalibrator::get_nstate_pulsar () const
{
  return 0;
}
    
//! Retern a new plot information interface for the specified pulsar state
Pulsar::Calibrator::Info* 
Pulsar::SystemCalibrator::new_info_pulsar (unsigned istate) const
{
  throw Error (InvalidState, "Pulsar::SystemCalibrator::new_info_pulsar",
	       "not implemented");
}

unsigned Pulsar::SystemCalibrator::get_nchan () const
{
  return model.size();
}

unsigned Pulsar::SystemCalibrator::get_ndata (unsigned ichan) const
{
  assert (ichan < model.size());
  return model[ichan]->get_equation()->get_ndata ();
}

using namespace MEAL;

void Pulsar::SystemCalibrator::set_gain( Univariate<Scalar>* f )
{
  gain_variation = f;
}

void Pulsar::SystemCalibrator::set_diff_gain( Univariate<Scalar>* f )
{
  diff_gain_variation = f;
}

void Pulsar::SystemCalibrator::set_diff_phase( Univariate<Scalar>* f )
{
  diff_phase_variation = f;
}

//! Add the observation to the set of constraints
void Pulsar::SystemCalibrator::add_observation (const Archive* data) try
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
  throw error += "Pulsar::SystemCalibrator::add_observation";
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::SystemCalibrator::add_pulsar (const Archive* data) try
{
  if (verbose)
    cerr << "Pulsar::SystemCalibrator::add_pulsar" << endl;

  match (data);

  Reference::To<Archive> clone;

  BackendCorrection correct_backend;
  if( correct_backend.required (data) )
  {
    if (verbose)
      cerr << "Pulsar::SystemCalibrator::add_pulsar correct backend" << endl;
    clone = data->clone();
    correct_backend (clone);
    data = clone;
  }

  unsigned nsub = data->get_nsubint ();

  get_data_fail = 0;
  get_data_call = 0;

  for (unsigned isub=0; isub<nsub; isub++)
    add_pulsar( data, isub );

  if (get_data_fail)
    cerr << "\t" << get_data_fail << " failures in " << get_data_call
	 << " data points" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::add_pulsar";
}

//! Add the specified pulsar observation to the set of constraints
void 
Pulsar::SystemCalibrator::add_pulsar (const Archive* data, unsigned isub) try
{
  const Integration* integration = data->get_Integration (isub);
  unsigned nchan = integration->get_nchan ();

  if (model.size() > 1 && nchan != model.size())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::add_pulsar",
		 "input data nchan=%d != model nchan=%d", nchan, model.size());

  MJD epoch = integration->get_epoch ();
  add_epoch (epoch);

  // use the FrontendCorrection class to calculate the feed transformation
  ProjectionCorrection correction;
  correction.set_archive (data);
  Jones<double> projection = correction (isub);

  cerr << correction.get_summary () << endl;

  // an identifier for this set of data
  string identifier = data->get_filename() + " " + tostring(isub);

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::add_pulsar identifier="
	 << identifier << endl;

  for (unsigned ichan=0; ichan<nchan; ichan++) try
  {
    if (integration->get_weight (ichan) == 0)
    {
      if (verbose > 2)
	cerr << "Pulsar::SystemCalibrator::add_observation ichan="
	     << ichan << " flagged invalid" << endl;
      continue;
    }
    
    unsigned mchan = ichan;
    if (model.size() == 1)
      mchan = 0;

    using MEAL::Argument;
    
    // epoch abscissa
    Argument::Value* time = model[mchan]->time.new_Value( epoch );
    
    // projection transformation
    Argument::Value* xform = model[mchan]->projection.new_Value( projection );
    
    // pulsar signal path
    unsigned path = model[mchan]->get_pulsar_path();
    
    // measurement set
    Calibration::CoherencyMeasurementSet measurements (path);
    
    measurements.set_identifier( identifier );
    measurements.add_coordinate( time );
    measurements.add_coordinate( xform );
    measurements.set_coordinates();
    
    try
    {
      get_data_call ++;

      if (verbose > 2)
	cerr << "Pulsar::SystemCalibrator::add_pulsar call add_pulsar ichan="
	     << ichan << endl;
  
      add_pulsar (measurements, integration, ichan);
    }
    catch (Error& error)
    {
      if (verbose > 2 || error.get_code() != InvalidParam)
        cerr << "Pulsar::SystemCalibrator::add_pulsar error" << error << endl;
      get_data_fail ++;
    }
    
    model[mchan]->add_observation_epoch (epoch);
    
  }
  catch (Error& error)
  {
    cerr << "Pulsar::SystemCalibrator::add_observation ichan="
	 << ichan << " error\n" << error.get_message() << endl;
  }
  
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::add_pulsar subint";
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::SystemCalibrator::match (const Archive* data)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::match",
		 "No Archive containing pulsar data has yet been added");

  string reason;
  if (!get_calibrator()->mixable (data, reason))
    throw Error (InvalidParam, "Pulsar::SystemCalibrator::match",
		 "'" + data->get_filename() + "' does not match "
		 "'" + get_calibrator()->get_filename() + reason);
}

//! Add the specified pulsar observation to the set of constraints
void Pulsar::SystemCalibrator::add_calibrator (const Archive* data)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::add_calibrator",
		 "No Archive containing pulsar data has yet been added");

  Reference::To<ReferenceCalibrator> polncal;

  if (model_type == Calibrator::Hamaker)
  {
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::add_calibrator"
	" new PolarCalibrator" << endl;
    
    polncal = new PolarCalibrator (data);
  }
  else
  {
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::add_calibrator"
	" new SingleAxisCalibrator" << endl;
    
    polncal = new SingleAxisCalibrator (data);
  }

  polncal->set_nchan( get_calibrator()->get_nchan() );

  add_calibrator (polncal);
}

//! Add the ReferenceCalibrator observation to the set of constraints
void 
Pulsar::SystemCalibrator::add_calibrator (const ReferenceCalibrator* p) try 
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::add_calibrator" << endl;

  const Archive* cal = p->get_Archive();

  if (cal->get_state() != Signal::Coherence)
    throw Error (InvalidParam, 
		 "Pulsar::SystemCalibrator::add_calibrator",
		 "Archive='" + cal->get_filename() + "' "
		 "invalid state=" + State2string(cal->get_state()));

  if ( cal->get_type() != Signal::FluxCalOn && 
       cal->get_type() != Signal::PolnCal )
    throw Error (InvalidParam,
                 "Pulsar::SystemCalibrator::add_calibrator",
                 "invalid source=" + Source2string(cal->get_type()));

  string reason;
  if (!get_calibrator()->calibrator_match (cal, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::add_calibrator",
		 "mismatch between calibrators\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + cal->get_filename() + reason);

  unsigned nchan = get_calibrator()->get_nchan ();
  unsigned nsub = cal->get_nsubint();
  unsigned npol = cal->get_npol();
  
  assert (npol == 4);

  Signal::Source source = cal->get_type();

  if (calibrator_estimate.source.size() == 0)
    create_calibrator_estimate( source );

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  epoch_added = vector<bool> (nchan, false);

  for (unsigned isub=0; isub<nsub; isub++)
  {
    const Integration* integration = cal->get_Integration (isub);

    MJD epoch = integration->get_epoch();
    add_epoch( epoch );

    ReferenceCalibrator::get_levels (integration, nchan, cal_hi, cal_lo);

    string identifier = cal->get_filename() + " " + tostring(isub);

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (integration->get_weight (ichan) == 0 || !model[ichan]->valid)
      {
	if (verbose > 2)
	  cerr << "Pulsar::SystemCalibrator::add_calibrator ichan="
	       << ichan << " flagged invalid" << endl;
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

      data.source = source;
      data.epoch = epoch;
      data.ichan = ichan;

      // convert to Stokes parameters
      data.observation = coherency( convert (calibtor) );
      data.baseline = coherency( convert (baseline) );

      try {

	Calibration::CoherencyMeasurementSet measurements;

	measurements.set_identifier( identifier );
	measurements.add_coordinate( model[ichan]->time.new_Value(epoch) );

        // convert to CoherencyMeasurement format
        Calibration::CoherencyMeasurement 
	  state (calibrator_estimate.input_index);

	state.set_stokes( data.observation );
        measurements.push_back( state );

	submit_calibrator_data (measurements, data);

      }
      catch (Error& error) {
        cerr << "Pulsar::SystemCalibrator::add_calibrator ichan="
             << ichan << " error\n" << error << endl;
	continue;
      }

      integrate_calibrator_data( p->get_response(ichan), data );

      if (p->get_nchan() == nchan && p->get_transformation_valid (ichan))
      {
	Signal::Source source = p->get_Archive()->get_type();
	model[ichan]->integrate_calibrator (p->get_transformation(ichan), 
					    source == Signal::FluxCalOn);
      }
    }
  }
}
 catch (Error& error) 
 {
   throw error += "Pulsar::SystemCalibrator::add_calibrator";
 }

void Pulsar::SystemCalibrator::init_estimate (SourceEstimate& estimate)
{
  unsigned nchan = get_calibrator()->get_nchan ();
  unsigned nbin = get_calibrator()->get_nbin ();

  if (estimate.phase_bin >= nbin)
    throw Error (InvalidRange, "Pulsar::SystemCalibrator::init_estimate",
		 "phase bin=%d >= nbin=%d", estimate.phase_bin, nbin);

  estimate.source.resize (nchan);
  estimate.source_guess.resize (nchan);

  bool first_channel = true;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->valid)
      continue;

#if 0
    if (physical_coherency)
      estimate.source[ichan] = new MEAL::PhysicalCoherency;
    else
#endif
      estimate.source[ichan] = new MEAL::Coherency;

    string prefix = "psr_" + tostring(estimate.phase_bin) + "_";
    estimate.source[ichan]->set_param_name_prefix( prefix );

    unsigned nsource = model[ichan]->get_equation()->get_num_input();

    if (first_channel)
      estimate.input_index = nsource;

    else if (estimate.input_index != nsource)
      throw Error (InvalidState, "Pulsar::SystemCalibrator::init_estimate",
		   "isource=%d != nsource=%d (ichan=%d)",
		   estimate.input_index, nsource, ichan);

    model[ichan]->get_equation()->add_input( estimate.source[ichan] );

    first_channel = false;
  }

}


void Pulsar::SystemCalibrator::create_calibrator_estimate ( Signal::Source )
{
  if (verbose)
    cerr << "Pulsar::SystemCalibrator::create_calibrator_estimate" << endl;

  // add the calibrator states to the equations
  init_estimate (calibrator_estimate);

  // set the initial guess and fit flags
  Stokes<double> cal_state (1,0,.5,0);

  Signal::Basis basis = get_calibrator()->get_basis ();

  if (basis == Signal::Circular)
    cal_state = Stokes<double> (1,.5,0,0);

  unsigned nchan = get_nchan();

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (calibrator_estimate.source[ichan])
    {   
      calibrator_estimate.source[ichan]->set_stokes( cal_state );
      calibrator_estimate.source[ichan]->set_infit( 0, false );

      calibrator_estimate.source[ichan]->set_param_name_prefix( "cal_" );
    }
}

void Pulsar::SystemCalibrator::submit_calibrator_data 
(
 Calibration::CoherencyMeasurementSet& measurements,
 const SourceObservation& data
 )
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::submit_calibrator_data ichan="
	 << data.ichan << endl;

  // it may be necessary to remove this signal path if
  // the add_data step fails and no other calibrator succeeds
  if (!model[data.ichan]->get_polncal_path())
  {
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::submit_calibrator_data add polncal"
	   << endl;
    model[data.ichan]->add_polncal_backend();
  }

  measurements.set_transformation_index
    ( model[data.ichan]->get_polncal_path() );

  if (!epoch_added[data.ichan])
  {
#ifdef _DEBUG
    cerr << "isub=" << isub 
	 << " add epoch[" << data.ichan << "]=" << epoch << endl;
#endif
    model[data.ichan]->add_calibrator_epoch (data.epoch);
    epoch_added[data.ichan] = true;
  }

  model[data.ichan]->get_equation()->add_data (measurements);
}

void Pulsar::SystemCalibrator::integrate_calibrator_data
(
 const Jones< Estimate<double> >& correct,
 const SourceObservation& data
 )
{
  Stokes< Estimate<double> > result = transform( data.observation, correct );
  calibrator_estimate.source_guess[data.ichan].integrate (result);
}


Pulsar::CalibratorStokes*
Pulsar::SystemCalibrator::get_CalibratorStokes () const
{
  if (calibrator_stokes)
    return calibrator_stokes;

  if (verbose > 2) cerr << "Pulsar::SystemCalibrator::get_CalibratorStokes"
		 " create CalibratorStokes Extension" << endl;

  unsigned nchan = get_nchan();

  if (nchan != calibrator_estimate.source.size())
    throw Error (InvalidState,
		 "Pulsar::SystemCalibrator::get_CalibratorStokes",
		 "Calibrator Stokes nchan=%d != Transformation nchan=%d",
		 calibrator_estimate.source.size(), nchan);

  Reference::To<CalibratorStokes> ext = new CalibratorStokes;
    
  ext->set_nchan (nchan);
    
  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    bool valid = get_transformation_valid(ichan);
      
    ext->set_valid (ichan, valid);
    if (!valid)
      continue;
    
    ext->set_stokes (ichan, calibrator_estimate.source[ichan]->get_stokes());
  }
  catch (Error& error)
  {
    cerr << "Pulsar::SystemCalibrator::get_CalibratorStokes ichan="
	 << ichan << " error\n" << error.get_message() << endl;
    ext->set_valid (ichan, false);
  }

  calibrator_stokes = ext;
  
  return calibrator_stokes;

}

void Pulsar::SystemCalibrator::create_model ()
{
  receiver = get_calibrator()->get<Receiver>();

  MEAL::Complex2* basis = 0;
  if (receiver)
  {
    /*
      If the calibrator is a calibrated standard, as is the case with
      the template used in matrix template matching, then its basis
      will have already been corrected.  The receiver should probably
      be taken from the first uncalibrated archive to be fit, or
      combined with the platform transformation.
    */
    if (receiver->get_basis_corrected())
    {
      Receiver* clone = receiver->clone();
      clone->set_basis_corrected (false);
      receiver = clone;
    }

    BasisCorrection basis_correction;
    basis = new MEAL::Complex2Constant (basis_correction(receiver));

    if (verbose)
      cerr << "Pulsar::SystemCalibrator::create_model receiver=\n  " 
	   << basis->evaluate() << endl;
  }

  unsigned nchan = get_calibrator()->get_nchan();
  model.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::create_model ichan=" << ichan << endl;

    bool britton = model_type == Calibrator::Britton;
    model[ichan] = new Calibration::StandardModel (britton);

    if (verbose > 2)
      cerr << "Pulsar::SystemCalibrator::create_model set variations" << endl;

    if (gain_variation)
      model[ichan]->set_gain( gain_variation->clone() );

    if (diff_gain_variation)
      model[ichan]->set_diff_gain( diff_gain_variation->clone() );

    if (diff_phase_variation)
      model[ichan]->set_diff_phase( diff_phase_variation->clone() );

    if (basis)
      model[ichan]->set_basis (basis);

    if (solver)
      model[ichan]->set_solver( solver->clone() );
  }

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::create_model exit" << endl;
}

//! Return the StandardModel for the specified channel
const Calibration::StandardModel*
Pulsar::SystemCalibrator::get_model (unsigned ichan) const
{
  return model[ichan];
}

void Pulsar::SystemCalibrator::set_nthread (unsigned nthread)
{
  queue.resize (nthread);
}


void Pulsar::SystemCalibrator::solve_prepare ()
{
  if (is_prepared)
    return;

  if (set_initial_guess)
    calibrator_estimate.update_source();

  MJD epoch = get_epoch();

  if (verbose)
    cerr << "Pulsar::SystemCalibrator::solve_prepare epoch=" << epoch << endl;

  for (unsigned ichan=0; ichan<model.size(); ichan++)
  {
    if (model[ichan]->get_equation()->get_ndata() == 0)
    {
      if (verbose)
	cerr << "Pulsar::SystemCalibrator::solve_prepare warning"
	  " ichan=" << ichan << " has no data" << endl;
      model[ichan]->valid = false;
    }

    if (!model[ichan]->valid)
      continue;

    if (ichan < calibrator_estimate.source.size())
    {
      // sanity check
      Estimate<double> I = calibrator_estimate.source[ichan]->get_stokes()[0];
      if (fabs(I.get_value()-1.0) > 1e-5 && verbose)
	cerr << "Pulsar::SystemCalibrator::solve_prepare warning"
	  " ichan=" << ichan << " reference flux=" << I << " != 1" << endl;
    }

    model[ichan]->set_reference_epoch ( epoch );
    
    model[ichan]->check_constraints ();
    
    if (set_initial_guess)
      model[ichan]->update ();
    
    if (verbose > 2)
      model[ichan]->get_equation()->get_solver()->set_debug();

  }

  is_prepared = true;
}


void Pulsar::SystemCalibrator::solve ()
{
  ReceptionModel::Solver::report_chisq = true;

  solve_prepare ();

  unsigned nchan = model.size();

  unsigned valid = 0;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->valid)
    {
      cerr << "channel " << ichan << " flagged invalid" << endl;
      continue;
    }

    valid ++;

    // first valid channel, print a report
    if (valid == 1)
      model[ichan]->get_equation()->get_solver()->set_report ();

    queue.submit( model[ichan].get(), &StandardModel::solve );

  }

  queue.wait ();

  // attempt to fix up any channels that didn't converge well
  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (!model[ichan]->valid)
      continue;

    ReceptionModel* equation = model[ichan]->get_equation();

    float chisq = equation->get_solver()->get_chisq ();
    unsigned free = equation->get_solver()->get_nfree ();
    float reduced_chisq = chisq/free;

    if (reduced_chisq > try_again_chisq)
    {
      cerr << "try for better fit in ichan=" << ichan 
	   << " chisq/nfree=" << reduced_chisq << endl;

      resolve (ichan);
    }
  }

  queue.wait ();

  covariance.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++) try
  {
    if (!model[ichan]->valid)
      continue;

    model[ichan]->get_covariance( covariance[ichan], get_epoch() );
  }
  catch (Error& error)
  {
    if (verbose)
      cerr << "Pulsar::SystemCalibrator::solve failure " 
           << error.get_message() << endl;
    model[ichan]->valid = false;
  }

  // ensure that calculate_transformation is called again
  transformation.resize (0);

  is_solved = true;
}

void Pulsar::SystemCalibrator::resolve (unsigned ichan) try
{
  unsigned nchan = model.size();

  // look for the nearest neighbour with a solution
  for (int off=1; off < int(nchan); off++)
  {
    for (int dir=-1; dir!=1; dir=1)
    {
      int jchan = int(ichan) + dir * off;

      if (jchan < 0 || jchan >= int(nchan))
	continue;

      if (!model[jchan]->valid)
	continue;

      ReceptionModel* equation = model[jchan]->get_equation();

      if (!equation->get_solver()->get_solved())
	continue;

      float chisq = equation->get_solver()->get_chisq ();
      unsigned free = equation->get_solver()->get_nfree ();
      float reduced_chisq = chisq/free;

      if (reduced_chisq > try_again_chisq)
	continue;
      
      cerr << "copying solution from jchan=" << jchan 
	   << " chisq/nfree=" << reduced_chisq << endl;

      model[ichan]->get_equation()->copy_fit( equation );      
      queue.submit( model[ichan].get(), &StandardModel::solve );

      return;
    }
  }

  cerr << "could not find a suitable solution to copy for retry" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::SystemCalibrator::resolve";
}

bool Pulsar::SystemCalibrator::get_prepared () const
{
  return is_prepared;
}

bool Pulsar::SystemCalibrator::get_solved () const
{
  return is_solved;
}

/*! Retrieves the transformation from the standard model in each channel */
void Pulsar::SystemCalibrator::calculate_transformation ()
{
  unsigned nchan = model.size();

  transformation.resize( nchan );

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    transformation[ichan] = 0;

    if (model[ichan]->valid)
      transformation[ichan] = model[ichan]->get_transformation();   
  }
}

//! Calibrate the polarization of the given archive
void Pulsar::SystemCalibrator::precalibrate (Archive* data)
{
  if (verbose > 2)
    cerr << "Pulsar::SystemCalibrator::precalibrate" << endl;

  string reason;
  if (!get_calibrator()->calibrator_match (data, reason))
    throw Error (InvalidParam, "Pulsar::PulsarCalibrator::precalibrate",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + data->get_filename() + reason);

  unsigned nsub = data->get_nsubint ();
  unsigned nchan = data->get_nchan ();

  // sanity check
  if (nchan != model.size())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::precalibrate",
                 "model size=%u != data nchan=%u", model.size(), nchan);

  vector< Jones<float> > response (nchan);

  bool parallactic_corrected = false;

  BackendCorrection correct_backend;
  correct_backend (data);

  for (unsigned isub=0; isub<nsub; isub++)
  {
    Integration* integration = data->get_Integration (isub);

    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (!model[ichan]->valid)
      {
	if (verbose > 2)
	  cerr << "Pulsar::SystemCalibrator::precalibrate ichan=" << ichan 
	       << " zero weight" << endl;

	integration->set_weight (ichan, 0.0);

	response[ichan] = Jones<double>::identity();
	continue;
      }

      MEAL::Complex2* signal_path = 0;
      ReceptionModel* equation = model[ichan]->get_equation();

      switch ( data->get_type() )  {
      case Signal::Pulsar:
	// cerr << "Pulsar::ReceptionCalibrator::precalibrate Pulsar" << endl;
        equation->set_transformation_index (model[ichan]->get_pulsar_path());
        signal_path = equation->get_transformation ();
	parallactic_corrected = true;
	break;
      case Signal::PolnCal:
	// cerr << "Pulsar::ReceptionCalibrator::precalibrate PolnCal" << endl;
        equation->set_transformation_index (model[ichan]->get_polncal_path());
        signal_path = equation->get_transformation ();
	break;
      case Signal::FluxCalOn:
	// cerr << "Pulsar::ReceptionCalibrator::precalibrate FluxCal" << endl;
        equation->set_transformation_index (model[ichan]->get_fluxcal_path());
        signal_path = equation->get_transformation ();
        break;
      default:
	throw Error (InvalidParam, "Pulsar::SystemCalibrator::precalibrate",
		     "unknown Archive type for " + data->get_filename() );
      }

      response[ichan] = Jones<float>::identity();

      if (!signal_path) {
        integration->set_weight (ichan, 0.0);
        continue;
      }

      try {
	model[ichan]->time.set_value( integration->get_epoch() );
#ifdef _DEBUG
	cerr << "para=" << model[ichan]->parallactic.get_parallactic_angle()
	     << endl;
#endif
	response[ichan] = signal_path->evaluate();
      }
      catch (Error& error) {
	if (verbose > 2)
	  cerr << "Pulsar::SystemCalibrator::precalibrate ichan=" << ichan
	       << endl << error.get_message() << endl;
        integration->set_weight (ichan, 0.0);
        response[ichan] = Jones<float>::identity();
	continue;
      }

      if ( norm(det( response[ichan] )) < 1e-9 ) {
        if (verbose > 2)
          cerr << "Pulsar::SystemCalibrator::precalibrate ichan=" << ichan
               << " faulty response" << endl;
        integration->set_weight (ichan, 0.0);
        response[ichan] = Jones<float>::identity();
	continue;
      }
      else
	response[ichan] = inv( response[ichan] );
    }

    integration->expert()->transform (response);
  }

  data->set_poln_calibrated (true);
  data->set_scale (Signal::ReferenceFluxDensity);

  Receiver* receiver = data->get<Receiver>();

  if (!receiver)
  {
    cerr << "Pulsar::SystemCalibrator::precalibrate WARNING: "
      "cannot record corrections" << endl;
    return;
  }

  receiver->set_projection_corrected (parallactic_corrected);
  receiver->set_basis_corrected (true);
}


Pulsar::Archive*
Pulsar::SystemCalibrator::new_solution (const string& class_name) const try
{
  if (verbose > 2) cerr << "Pulsar::SystemCalibrator::new_solution"
    " create CalibratorStokes Extension" << endl;

  Reference::To<Archive> output = Calibrator::new_solution (class_name);
  
  if (calibrator_estimate.source.size())
  {
    Reference::To<CalibratorStokes> stokes = get_CalibratorStokes();
    output -> add_extension (stokes);
  }

  return output.release();

}
catch (Error& error) {
  throw error += "Pulsar::SystemCalibrator::new_solution";
}

