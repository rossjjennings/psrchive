/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/StandardSpectra.h"

#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/TotalCovariance.h"

#include "MEAL/PhaseGradients.h"
#include "MEAL/Complex2Math.h"
#include "MEAL/Complex2Constant.h"

#include "RealTimer.h"
#include "Pauli.h"
#include "FTransform.h"

// #define _DEBUG 1

#include <memory>

using namespace std;
using namespace Pulsar;
using namespace Calibration;

bool PolnProfileFit::verbose = false;

//! Default constructor
PolnProfileFit::PolnProfileFit ()
{
  init ();
}

//! Copy constructor
PolnProfileFit::PolnProfileFit (const PolnProfileFit& fit)
{
  init ();
  operator = (fit);
}

/*! This operator is under development and is not fit for use */
PolnProfileFit&
PolnProfileFit::operator = (const PolnProfileFit& fit)
{
  cerr << "PolnProfileFit::operator = WARNING not implemented" << endl;

  choose_maximum_harmonic = fit.choose_maximum_harmonic;

  manage_equation_transformation = fit.manage_equation_transformation;

  maximum_harmonic = fit.maximum_harmonic;
  n_harmonic = fit.n_harmonic;

  standard = fit.standard;
  standard_fourier = fit.standard_fourier;

  standard_data = fit.standard_data;

  fit_debug = fit.fit_debug;
  standard_variance = fit.standard_variance;
  regions_set = fit.regions_set;

  return *this;
}

//! Destructor
PolnProfileFit::~PolnProfileFit ()
{
}

PolnProfileFit* PolnProfileFit::clone () const
{
  return new PolnProfileFit (*this);
}

void PolnProfileFit::set_normalize_by_invariant (bool set)
{
  standard_data->set_normalize (set);
}

void PolnProfileFit::init ()
{
  standard_data = new StandardSpectra;

  phases = new MEAL::PhaseGradients<MEAL::Complex2>;

  // connect to the phase axis via the Univariate<> interface
  phases -> set_argument (0, &phase_axis);

  // connect to the index axis via the standard Callback interface
  index_axis.signal.connect (phases, &MEAL::PhaseGradients<MEAL::Complex2>::set_igradient);

  maximum_harmonic = n_harmonic = 0;

  regions_set = false;
  choose_maximum_harmonic = false;
  manage_equation_transformation = true;
  fit_debug = false;
  phase_lock = false;
  shared_phase = false;
}

void PolnProfileFit::set_plan (FTransform::Plan* p)
{
  get_spectra()->get_stats()->set_plan (p);
}

void PolnProfileFit::set_maximum_harmonic (unsigned max)
{
  maximum_harmonic = max;
}

//! Get the standard to which observations will be fit
const PolnProfile* PolnProfileFit::get_standard () const
{
  return standard; 
}

//! separate the values and the variances
void valvar( const Stokes< complex< Estimate<double> > >& data,
	     Stokes< complex<double> >& val,
	     Stokes< complex<double> >& var )
{
  for (unsigned ipol=0; ipol < 4; ipol++)
  {
    val[ipol] = complex<double>( data[ipol].real().get_value(),
				 data[ipol].imag().get_value() );

    var[ipol] = complex<double>( data[ipol].real().get_variance(),
				 data[ipol].imag().get_variance() );
  }
}

void PolnProfileFit::set_regions (const PhaseWeight& on, const PhaseWeight& off)
{
  get_spectra()->get_stats()->set_regions ( on, off );
  regions_set = true;
}

//! Set the standard to which observations will be fit
void PolnProfileFit::set_standard (const PolnProfile* _standard)
{
  standard = _standard;

  // until greater resize functionality is added to ReceptionModel,
  // best to just delete it and start fresh
  equation = 0;
  
  if (!standard)
    return;

#ifdef _DEBUG
  cerr << "PolnProfileFit::set_standard set profile" << endl;
#endif

  if (regions_set)
    standard_data->set_profile (standard);
  else
    standard_data->select_profile (standard);

  standard_fourier = standard_data->get_fourier();

  // number of complex phase bins in Fourier domain
  unsigned std_harmonic = standard->get_nbin() / 2;

#ifdef _DEBUG
  cerr << "PolnProfileFit::set_standard max harmonic" << endl;
#endif

  if (choose_maximum_harmonic)
  {
    n_harmonic = standard_data->get_last_harmonic ();
    if (verbose)
      cerr << "PolnProfileFit::set_standard chose "
	   << n_harmonic << " harmonics" << endl;
  }
  else if (maximum_harmonic && maximum_harmonic < std_harmonic)
  {
    n_harmonic = maximum_harmonic;
    if (verbose)
      cerr << "PolnProfileFit::set_standard using " << maximum_harmonic
	   << " out of " << std_harmonic << " harmonics" << endl;
  }
  else
  {
    n_harmonic = std_harmonic;
    if (verbose)
      cerr << "PolnProfileFit::set_standard using all "
	   << std_harmonic << " harmonics" << endl;
  }

#ifdef _DEBUG
  cerr << "PolnProfileFit::set_standard create ReceptionModel" << endl;
#endif

  equation = new ReceptionModel;

  // equation->set_fit_debug( fit_debug );

#ifdef _DEBUG
  cerr << "PolnProfileFit::set_standard created" << endl;
#endif

  if (manage_equation_transformation && transformation)
  {
#ifndef _DEBUG
    if (verbose)
#endif
      cerr << "PolnProfileFit::set_standard init transformation" << endl;
    // initialize the model transformation
    equation->set_transformation (transformation);
  }

#ifdef _DEBUG
  cerr << "PolnProfileFit::set_standard resize " << n_harmonic - 1 << endl;
#endif

  uncertainty.resize (n_harmonic - 1);

  // initialize the model input states

  for (unsigned ibin=1; ibin<n_harmonic; ibin++)
  {
#ifdef _DEBUG
    cerr << "PolnProfileFit::set_standard ibin=" << ibin << endl;
#endif

    Stokes< complex<double> > standard_value;

    valvar( standard_data->get_stokes(ibin),
	    standard_value, standard_variance );

#ifdef _DEBUG
    cerr << "PolnProfileFit::set_standard create Constant" << endl;
#endif

    // each complex phase bin of the standard is treated as a known constant
    MEAL::Complex2Constant* jones;
    jones = new MEAL::Complex2Constant( convert(standard_value) );

    uncertainty[ibin-1] = new TemplateUncertainty;
    uncertainty[ibin-1] -> set_template_variance (standard_variance);

    if (transformation)
      uncertainty[ibin-1]->set_transformation (transformation);

    DEBUG("PolnProfileFit::set_standard set things" << ibin);

    if (phases)
    {
      // each complex phase bin is phase related
      Reference::To<MEAL::Complex2> input = jones;
      Reference::To<MEAL::Complex2> ph = phases.get();
      equation->add_input( input * ph );
    }
    else
      equation->add_input( jones );
  }
}


//! Set the transformation between the standard and observation
void PolnProfileFit::set_transformation (MEAL::Complex2* xform)
{
  transformation = xform;

  for (unsigned i=0; i<uncertainty.size(); i++)
    uncertainty[i]->set_transformation (xform);

  if (equation && manage_equation_transformation)
    equation->set_transformation (xform);
}

MEAL::Complex2* PolnProfileFit::get_transformation () const
{
  return transformation;
}

void PolnProfileFit::set_fit_debug (bool flag)
{
  fit_debug = flag;

  if (equation)
    equation->get_solver()->set_debug (flag);
}

void PolnProfileFit::set_phase_lock (bool locked)
{
  phase_lock = locked;
}

void PolnProfileFit::remove_phase ()
{
  if (!phases)
    return;

  delete phases;
}

void PolnProfileFit::share_phase ()
{
  if (!phases)
    throw Error (InvalidState, "PolnProfileFit::share_phase",
		 "cannot call share_phase after remove_phase has been called");

  if (phases->get_ngradient() > 1)
    throw Error( InvalidState, "PolnProfileFit::share_phase",
		 "cannot call share_phase when ngradient=%u > 1",
		 phases->get_ngradient() );

  shared_phase = true;
}

//! Fit the specified observation to the standard
void PolnProfileFit::fit (const PolnProfile* observation) try
{
  set_observation (observation);
  solve ();
}
catch (Error& error)
{
  throw error += "PolnProfileFit::fit";
}

void PolnProfileFit::delete_observations ()
{
   // delete any previously set data
  equation->delete_data ();

  // (re)set the number of phases to zero
  if (phases)
    phases->resize (0);
}

void PolnProfileFit::set_observation (const PolnProfile* only) try
{
  if (!equation)
    throw Error (InvalidState, "PolnProfileFit::set_observation",
		 "no measurement equation");

  delete_observations ();
  add_observation (only);
}
catch (Error& error)
{
  throw error += "PolnProfileFit::set_observation";
}

void PolnProfileFit::set_measurement_set
( const CoherencyMeasurementSet& the_template)
{
  measurement_set = the_template;
  if (verbose)
    cerr << "PolnProfileFit::set_measurement_set path index="
         << measurement_set.get_transformation_index() << endl;
}

void PolnProfileFit::add_observation( const PolnProfile* observation )
{
  CoherencyMeasurementSet measurements (measurement_set);
  add_observation (measurement_set, observation);
  equation->add_data( measurements );
}

void PolnProfileFit::add_observation (CoherencyMeasurementSet& measurements,
				      const PolnProfile* observation )
{
  if (!standard)
    throw Error (InvalidState, "PolnProfileFit::add_observation",
		 "no standard specified.  call set_standard");

  if (!transformation)
    throw Error (InvalidState, "PolnProfileFit::add_observation",
		 "no transformation specified.  call set_transformation");

  if (!observation)
    throw Error (InvalidState, "PolnProfileFit::add_observation",
		 "no observation supplied as argument");

  // ensure that the PolnProfile class is cleaned up
  Reference::To<const PolnProfile> obs = observation;

  unsigned obs_harmonic = observation->get_nbin() / 2;

  if (obs_harmonic < n_harmonic)
    throw Error (InvalidState, "PolnProfileFit::add_observation",
		 "observation n_harmonic=%d < n_harmonic=%d",
		 obs_harmonic, n_harmonic);

  standard_data->set_profile( observation );

  Reference::To<const PolnProfile> fourier = standard_data->get_fourier();

  float phase_guess = ccf_max_phase (standard_fourier->get_Profile(0),
				     fourier->get_Profile(0));

  if (verbose)
    cerr << "PolnProfileFit::add_observation add gradient" << endl;

  unsigned index = 0;
  bool gradient_added = false;

  if (phases)
  {
    if (!shared_phase || phases->get_ngradient() == 0)
    {
      phases->add_gradient();
      gradient_added = true;
    }

    index = phases->get_igradient();

    phases->set_infit (index, !phase_lock);

    // TO-DO: when sharing phase, retain only the best phase_guess
    phases->set_param (index, phase_guess);
  }

  try
  {
    unsigned nbin_std = standard->get_nbin();
    unsigned nbin_obs = observation->get_nbin();

    /* 
       If the standard (template) and observed profiles have different
       numbers of bins, account for the offset between the centres of
       bin 0 of each profile.
       
       This will likely cause trouble if phases have been removed from
       the model.
       
       A potential fix: multiply the observation by the required phase
       gradient.
    */
    if (phases && nbin_std != nbin_obs)
      phases->set_offset (index, 0.5/nbin_std - 0.5/nbin_obs);
    
    // initialize the measurement sets
    for (unsigned ibin=1; ibin<n_harmonic; ibin++)
    {
      Stokes< complex<double> > val;
      Stokes< complex<double> > var;
      valvar( standard_data->get_stokes(ibin), val, var );
      
      TemplateUncertainty* error = uncertainty[ibin-1]->clone();
      error -> set_variance (var);
      
#ifdef _DEBUG
      if (error->get_transformation() != transformation)
	{
	  cerr << "error.xform=" << error->get_transformation() << " != "
	       << transformation.get() << endl;
	}
      else
	cerr << "clone ok!" << endl;
#endif
      
      CoherencyMeasurement measurement (ibin-1);
      measurement.set_stokes (val, error);
      
      double phase_shift = -2.0 * M_PI * double(ibin);

      measurement.add_coordinate ( phase_axis.new_Value(phase_shift) );
      measurement.add_coordinate ( index_axis.new_Value(index) );
      
      measurements.push_back ( measurement );
    }
  }
  catch (Error& error)
  {
    if (gradient_added)
      phases->remove_gradient();

    throw error += "PolnProfileFit::add_observation";
  }
}


void PolnProfileFit::solve () try
{
  RealTimer clock;

  clock.start();

  equation->solve ();

  clock.stop();

  if (verbose)
  {
    float chisq = equation->get_solver()->get_chisq();
    float nfree = equation->get_solver()->get_nfree();
    cerr << "PolnProfileFit::solve solved in " << clock << "."
      " chisq=" << chisq/nfree << endl;
  }
}
catch (Error& error) {
  throw error += "PolnProfileFit::solve";
}

double PolnProfileFit::get_reduced_chisq () const
{
  ReceptionModel::Solver* solver = equation->get_solver();
  return solver->get_chisq() / solver->get_nfree();
}

//! Get the measurement equation used to model the fit
ReceptionModel* 
PolnProfileFit::get_equation ()
{
  return equation;
}

//! Get the measurement equation used to model the fit
const ReceptionModel* 
PolnProfileFit::get_equation () const
{
  return equation;
}

//! Get the statistical interface to the data
StandardSpectra* PolnProfileFit::get_spectra ()
{
  return standard_data;
}

//! Get the phase offset between the observation and the standard
Estimate<double> PolnProfileFit::get_phase () const
{
  if (!phases)
    return 0;

  if (phases->get_ngradient() == 0)
    throw Error (InvalidState, "PolnProfileFit::get_phase",
                 "no phases have been added to the equation");

  return phases -> get_Estimate (0);
}

//! Get the phase offset between the observation and the standard
void PolnProfileFit::set_phase (const Estimate<double>& value)
{
  if (!phases)
    return;

  if (phases->get_ngradient() == 0)
    throw Error (InvalidState, "PolnProfileFit::get_phase",
		 "no phases have been added to the equation");

  phases->set_Estimate (0, value);
}

/*!
  Calculates the shift between
  Returns a basic Tempo::toa object
*/
Tempo::toa PolnProfileFit::get_toa (const PolnProfile* observation,
					    const MJD& mjd, 
					    double period,
					    const string& nsite) try 
{
  if (verbose)
    cerr << "PolnProfileFit::get_toa" << endl;

  fit (observation);
  
  Estimate<double> pulse_phase = get_phase();

  Tempo::toa retval (Tempo::toa::Parkes);

  retval.set_frequency (observation->get_Profile(0)->get_centre_frequency());
  retval.set_arrival   (mjd + pulse_phase.val * period);
  retval.set_error     (sqrt(pulse_phase.var) * period * 1e6);

  retval.set_telescope (nsite);

  return retval;
}
catch (Error& error) {
  throw error += "PolnProfileFit::get_toa";
}

float PolnProfileFit::ccf_max_phase (const Profile* std,
					     const Profile* obs) const try
{
  const unsigned nbin = std::min (obs->get_nbin(), std->get_nbin()) / 2;

  const complex<float>* cstd
    = reinterpret_cast< const complex<float>* >( std->get_amps() );
  const complex<float>* cobs
    = reinterpret_cast< const complex<float>* >( obs->get_amps() );

  // calculate the cross power spectral density
  vector< complex<float> > cpsd (nbin);

  // don't care about DC term
  cpsd[0] = 0.0;

  for (unsigned ibin=1; ibin < nbin; ibin++)
    cpsd[ibin] = cobs[ibin] * conj(cstd[ibin]);

  // calculate the (complex) cross correlation function
  vector< complex<float> > ccf (nbin);

  FTransform::bcc1d ( nbin,
		      reinterpret_cast<float*>( &(ccf[0]) ),
		      reinterpret_cast<float*>( &(cpsd[0]) ) );

  // find the maximum modulus
  float max = 0;
  float imax = 0;
  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    float mod = norm (ccf[ibin]);
    if (mod > max)
    {
      max = mod;
      imax = ibin;
    }
  }

  if (imax > nbin/2)
    imax -= nbin;

  return float(imax)/nbin;
}
catch (Error& error)
{
  throw error += "PolnProfileFit::ccf_max_phase";
}
