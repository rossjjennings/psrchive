/***************************************************************************
 *
 *   Copyright (C) 2003-2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/BasisCorrection.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/SingleAxisSolver.h"
#include "Pulsar/SingleAxis.h"

#include "Pulsar/BackendFeed.h"
#include "Pulsar/VariableBackend.h"
#include "Pulsar/MeanJones.h"

#include "MEAL/Value.h"
#include "Pauli.h"

using namespace std;

Pulsar::HybridCalibrator::HybridCalibrator (const Archive* data)
{
  type = new CalibratorTypes::ovhb04;

  set_precalibrator( new Pulsar::PolnCalibrator (data) );
  set_reference_input( data->get<CalibratorStokes>() );
}

Pulsar::HybridCalibrator::HybridCalibrator (PolnCalibrator* _calibrator)
{
  type = new CalibratorTypes::ovhb04;

  set_precalibrator (_calibrator);
}

//! Destructor
Pulsar::HybridCalibrator::~HybridCalibrator ()
{
}

MJD Pulsar::HybridCalibrator::get_epoch () const
{
  return reference_observation->get_epoch();
}

//! Set the Stokes parameters of the reference signal
void
Pulsar::HybridCalibrator::set_reference_input (const CalibratorStokes* input,
    std::string filename)
{
  reference_input = input;

  if (!filename.empty())
  {
    filenames.resize(2);
    filenames[0] = filename;
  }
}

//! Set the ReferenceCalibrator data from which to derive a SingleAxis model
void
Pulsar::HybridCalibrator::set_reference_observation (const ReferenceCalibrator*
						     observation)
{
  reference_observation = observation;

  if (!precalibrator) set_calibrator ( observation->get_Archive() );

  filenames.resize (2);
  filenames[1] = observation->get_filenames();
}

const Pulsar::ReferenceCalibrator*
Pulsar::HybridCalibrator::get_reference_observation ()
{
  return reference_observation;
}

//! Set the PolnCalibrator to be supplemented by the SingleAxis model
void Pulsar::HybridCalibrator::set_precalibrator (PolnCalibrator* _calibrator)
{
  precalibrator = _calibrator;

  if (!precalibrator) return;

  copy_variation (precalibrator);
  
  set_calibrator( precalibrator->get_Archive() );

  filenames.resize (2);
  filenames[0] = precalibrator->get_filenames();
}


unsigned Pulsar::HybridCalibrator::get_maximum_nchan () const try
{
  unsigned maximum_nchan = 0;

  if (precalibrator)
    maximum_nchan = precalibrator->get_nchan();

  if (maximum_nchan != 0)
    return maximum_nchan;

  if (reference_observation)
    maximum_nchan = reference_observation->get_nchan();

  if (maximum_nchan != 0)
    return maximum_nchan;

  if (reference_input)
    return reference_input->get_nchan();

  throw Error (InvalidState, "Pulsar::HybridCalibrator::maximum_nchan",
	       "none of the required data have been added");
}
catch (Error& error)
{
  throw error += "Pulsar::HybridCalibrator::get_maximum_nchan";
}

unsigned Pulsar::HybridCalibrator::get_nchan () const try
{
  if (transformation.size() != 0)
    return transformation.size();

  return get_maximum_nchan ();
}
catch (Error& error)
{
  throw error += "Pulsar::HybridCalibrator::get_nchan";
}

//! Return the Stokes parameters of the ichan'th target channel
/*! \param target_nchan the number of frequency channels of the target
  If target_nchan is less than the number of frequency channels in the 
  CalibratorStokes instance, then neighbouring frequency channels will
  be integrated.
*/
Stokes< Estimate<double> > 
get_stokes (const Pulsar::CalibratorStokes* stokes,
	    const unsigned ichan, unsigned target_nchan)
{
  const unsigned have_nchan = stokes->get_nchan();
  const unsigned factor = have_nchan / target_nchan;

  if (!factor)
    throw Error (InvalidParam, "get_stokes",
		 "CalibratorStokes nchan=%u < required=%u",
		 have_nchan, target_nchan);

  Stokes< MeanEstimate<double> > result;
  unsigned stokes_ichan = ichan * factor;
  bool valid = false;

  for (unsigned i=0; i<factor; i++)
  {
    if (!stokes->get_valid (stokes_ichan+i))
    {
      if (Pulsar::Calibrator::verbose > 2)
	cerr << "get_stokes(CalibratorStokes) ichan=" << stokes_ichan+i
	     << " flagged invalid" << endl;
      continue;
    }

    result += stokes->get_stokes (stokes_ichan+i);
    valid = true;
  }

  if (!valid)
    throw Error (InvalidParam, "get_stokes(CalibratorStokes)",
		 "no valid data for ichan=%d", ichan);

  // MeanEstimate sets Stokes I to zero because the estimates have no error
  Stokes< Estimate<double> > answer = result;
  answer[0] = 1.0;

  return answer;
}


const MEAL::Complex2*
get_xform (const Pulsar::PolnCalibrator* response,
	   const unsigned ichan, bool backend_only)
{
  DEBUG("get_xform PolnCalibrator=" << (void*) response << "backend_only=" << backend_only);

  const MEAL::Complex2* xform = response->get_transformation(ichan);

  if (!backend_only)
    return xform;

  auto splittable = dynamic_cast<const Calibration::BackendFeed*> (xform);

  if (!splittable)
    throw Error (InvalidParam, "get_xform",
		 "transformation cannot be split into backend and feed");

  const MEAL::Complex2* backend = splittable->get_backend()->get_backend();

  DEBUG("get_xform backend name=" << backend->get_name() << " xform=" << backend->evaluate());
  return backend;
}

  
Jones<double> get_response (const Pulsar::PolnCalibrator* response,
			    const unsigned ichan, unsigned target_nchan,
			    bool backend_only = false) try
{
  const unsigned have_nchan = response->get_nchan();
  const unsigned factor = have_nchan / target_nchan;

  if (!factor)
    throw Error (InvalidParam, "get_response",
		 "PolnCalibrator nchan=%u < required=%u",
		 have_nchan, target_nchan);

  if (Pulsar::Calibrator::verbose > 2)
    cerr << "get_response(PolnCalibrator) ichan=" << ichan << endl;

  if (factor == 1)
  {
    DEBUG("get_response valid=" << response->get_transformation_valid (ichan));

    if (!response->get_transformation_valid (ichan))
      throw Error (InvalidParam, "get_response(PolnCalibrator)",
		   "ichan=%u flagged invalid", ichan);
    
    return get_xform(response, ichan, backend_only)->evaluate();
  }

  unsigned response_ichan = ichan * factor;
  bool valid = false;

  Calibration::MeanJones mean;
  Calibration::MeanJones::verbose = Pulsar::Calibrator::verbose > 2;

  for (unsigned i=0; i<factor; i++)
  {
    if (!response->get_transformation_valid (response_ichan+i))
    {
      if (Pulsar::Calibrator::verbose > 2)
	cerr << "get_response(PolnCalibrator) ichan=" << response_ichan+i
	     << " flagged invalid" << endl;
      continue;
    }

    mean.integrate( get_xform(response, response_ichan+i, backend_only) );

    valid = true;
  }

  if (!valid)
    throw Error (InvalidParam, "get_response(PolnCalibrator)",
		 "no valid data for ichan=%d", ichan);

  return mean.get_mean ();
}
catch (Error& error)
{
  throw error += "Jones<double> get_response";
}

template<typename T>
Jones<T> val (const Jones< Estimate<T> >& J)
{
  return Jones<T> (complex<T>(J(0,0).real().val, J(0,0).imag().val),
		   complex<T>(J(0,1).real().val, J(0,1).imag().val),
		   complex<T>(J(1,0).real().val, J(1,0).imag().val),
		   complex<T>(J(1,1).real().val, J(1,1).imag().val));
}

/*!

  If the reference observation and the pulsar observation have more
  frequency channels than the precalibrator, then the reference
  observation will be integrated down to the resolution of the
  precalibrator and the final solution will be interpolated back up to
  the resolution of the pulsar observation.  In this case, it is
  desireable to flag bad channels at the original resolution of the
  reference observation.

*/
bool Pulsar::HybridCalibrator::get_valid (unsigned ichan) const try
{
  bool valid = true;

  unsigned ref_nchan = 0;
  unsigned precal_nchan = 0;

  if (reference_observation)
    ref_nchan = reference_observation->get_nchan();

  if (precalibrator)
    precal_nchan = precalibrator->get_nchan();

  if ( precal_nchan &&
       observation_nchan > precal_nchan &&
       ref_nchan > precal_nchan )
  {
    if (verbose > 2)
      cerr << "Pulsar::HybridCalibrator::get_valid nchan: observation="
	   << observation_nchan << " and reference=" << ref_nchan
	   << " > precalibrator=" << precal_nchan << endl;

    if (ref_nchan <= observation_nchan)
    {
      /*
	if the reference observation channel is valid, then the
	corresponding (observation_nchan/ref_nchan) channels of the
	calibrated observation are also valid.
      */
      valid = reference_observation->get_transformation_valid
	( (ichan * ref_nchan) / observation_nchan );
    }
    else 
    {
      /*
	if any of the (ref_chan/observation_nchan) reference
	observation channels are valid, then the corresponding channel
	of the calibrated observation is also valid.
       */

      valid = false;
      unsigned factor = ref_nchan / observation_nchan;
      unsigned jchan_start = ichan * factor;
      unsigned jchan_end = jchan_start + factor;
      for (unsigned jchan=jchan_start; jchan<jchan_end; jchan++)
	if (reference_observation->get_transformation_valid(jchan))
	  valid = true;
    }
  }

  DEBUG("HybridCalibrator ichan=" << ichan << " valid=" << valid << " PolnCalibrator::valid=" << PolnCalibrator::get_valid (ichan));

  return valid && PolnCalibrator::get_valid (ichan);
}
catch (Error& error)
{ 
  throw error += "Pulsar::HybridCalibrator::get_valid";
}

void Pulsar::HybridCalibrator::calculate_transformation () try
{
  if (!reference_input)
    cerr << "Pulsar::HybridCalibrator::calculate_transformation\n"
            "  WARNING: no reference source input Stokes parameters" << endl;

  if (!reference_observation)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "no reference observation ReferenceCalibrator");

  unsigned target_nchan = get_maximum_nchan();
  if (observation_nchan && observation_nchan < target_nchan)
    target_nchan = observation_nchan;
  
  if (verbose > 2)
    cerr << "Pulsar::HybridCalibrator::calculate_transformation"
      " target_nchan=" << target_nchan << endl;

  // the calibrator hi and lo levels from the PolnCal archive
  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  reference_observation->get_levels (target_nchan, cal_hi, cal_lo);

  const unsigned npol = cal_hi.size();

  // coherency products in a single channel
  vector<Estimate<double> > cal (npol);

  // get the Receiver correction, if any
  const Receiver* receiver = 0;

  // Try the reference observation's version first ...
  if (reference_observation->has_Receiver())
    receiver = reference_observation->get_Receiver();

  // ... otherwise, try using precalibrator's version
  else if (precalibrator && precalibrator->has_Receiver())
    receiver = precalibrator->get_Receiver();

  Jones<double> basis_correction (1.0);

  if (receiver)
  {
    BasisCorrection correction;
    basis_correction = correction (receiver);
  }

  transformation.resize (target_nchan);

  // the correction to supplement the PolnCalibrator
  Reference::To<Calibration::SingleAxis> correction;

  for (unsigned ichan=0; ichan<target_nchan; ++ichan) try
  {
    if (verbose > 2)
      cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	" ichan=" << ichan << endl;

    // get the coherency vector of the measured reference source
    for (unsigned ipol=0; ipol<npol; ++ipol)
    {
      cal[ipol] = cal_hi[ipol][ichan];
      cal[ipol] -= cal_lo[ipol][ichan];
    }

    // get the Stokes parameters of the reference source observation
    Stokes< Estimate<double> > output_stokes = coherency (convert (cal));

    /* The following line provides a basis-independent representation of a
     reference source that illuminates both receptors equally and in phase. */
    Quaternion<double,Hermitian> ideal (1,0,1,0);

    Stokes< Estimate<double> > cal_stokes = standard (ideal);

    // get the Stokes parameters of the reference source input
    if (reference_input)
    {
      cal_stokes = get_stokes (reference_input, ichan, target_nchan);
      if (verbose > 2)
	cerr << "HybridCalibrator ichan=" << ichan 
             << " cal_stokes=" << cal_stokes << endl;
    }

    // get the precalibrator transformation
    Jones<double> response (1.0);

    if (precalibrator)
    {
      response = ::get_response (precalibrator, ichan, target_nchan);
      if (verbose > 2)
	cerr << "HybridCalibrator ichan=" << ichan 
             << " precal response=" << response << endl;
    }

    Jones<double> cal_response (1.0);

    switch (reference_input->get_coupling_point())
    {
    case CalibratorStokes::BeforeBasis:
      cal_response = response * basis_correction;
      break;

    case CalibratorStokes::BeforeFrontend:
      cal_response = response;
      break;
      
    case CalibratorStokes::BeforeIdeal:
      if (precalibrator)
	cal_response = ::get_response (precalibrator, ichan, target_nchan, true);
      break;

    default:
      string point = tostring(reference_input->get_coupling_point());
      throw Error (InvalidState, "HybridCalibrator::calculate_transformation",
		   "unknown coupling point=" + point);
    }

    if (verbose > 2)
      cerr << "HybridCalibrator ichan=" << ichan << " cal_response=" << cal_response << endl;
 
    // pass the reference Stokes parameters through the instrument
    Stokes< Estimate<double> > input_stokes;
    input_stokes = transform (cal_stokes, cal_response);

    // solve for the SingleAxis model that relates input and output states
    correction = new Calibration::SingleAxis;

    if (!solver)
      solver = new ::Calibration::SingleAxisSolver;

    solver->set_input (input_stokes);
    solver->set_output (output_stokes);
    solver->solve (correction);

    MEAL::Complex2* result = correction;

    if (precalibrator)
    {
      /* the result is the product of the correction
	 and the precalibrator response */

      MEAL::ProductRule<MEAL::Complex2>* product;
      product = new MEAL::ProductRule<MEAL::Complex2>;

      product->add_model( correction );
      product->add_model( new MEAL::Value<MEAL::Complex2>(response) );

      result = product;
    }
    
    transformation[ichan] = result;

  }
  catch (Error& error)
  {
    if (verbose > 1)
      cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	" error ichan=" << ichan << error << endl;
    transformation[ichan] = 0;
  }

  if (verbose > 2)
    cerr << "Pulsar::HybridCalibrator::calculate_transformation exit" << endl;
}
catch (Error& error)
{
  throw error += "Pulsar::HybridCalibrator::calculate_transformation";
}

