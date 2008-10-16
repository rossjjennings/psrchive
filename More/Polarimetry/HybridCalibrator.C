/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/BasisCorrection.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/SingleAxisSolver.h"
#include "Pulsar/SingleAxis.h"
#include "Pauli.h"

using namespace std;

Pulsar::HybridCalibrator::HybridCalibrator (const Archive* data)
{
  set_precalibrator( new Pulsar::PolnCalibrator (data) );
  set_reference_input( data->get<CalibratorStokes>() );
}

Pulsar::HybridCalibrator::HybridCalibrator (PolnCalibrator* _calibrator)
{
  set_precalibrator (_calibrator);
}

//! Destructor
Pulsar::HybridCalibrator::~HybridCalibrator ()
{
}

//! Return Calibrator::Hybrid
Pulsar::Calibrator::Type Pulsar::HybridCalibrator::get_type () const
{
  return Hybrid;
}

MJD Pulsar::HybridCalibrator::get_epoch () const
{
  return reference_observation->get_epoch();
}

//! Set the Stokes parameters of the reference signal
void
Pulsar::HybridCalibrator::set_reference_input (const CalibratorStokes* input)
{
  reference_input = input;
}

//! Set the ReferenceCalibrator data from which to derive a SingleAxis model
void Pulsar::HybridCalibrator::set_reference_observation (ReferenceCalibrator*
							  observation)
{
  reference_observation = observation;

  if (!precalibrator) set_calibrator ( observation->get_Archive() );

  filenames.resize (2);
  filenames[1] = observation->get_filenames();
}
    
//! Set the PolnCalibrator to be supplemented by the SingleAxis model
void Pulsar::HybridCalibrator::set_precalibrator (PolnCalibrator* _calibrator)
{
  precalibrator = _calibrator;

  if (!precalibrator) return;

  set_calibrator( precalibrator->get_Archive() );

  // store the Receiver Extension, if any
  receiver = get_calibrator()->get<Receiver>();

  filenames.resize (2);
  filenames[0] = precalibrator->get_filenames();
}

unsigned Pulsar::HybridCalibrator::get_maximum_nchan ()
{
  if (precalibrator)
    return precalibrator->get_nchan();

  if (reference_observation)
    return reference_observation->get_nchan();

  if (reference_input)
    return reference_input->get_nchan();

  throw Error (InvalidState, "Pulsar::HybridCalibrator::maximum_nchan",
	       "none of the required data have been added");
}

void Pulsar::HybridCalibrator::calculate_transformation ()
{
#if 0 
  if (!precalibrator)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "no precalibrator PolnCalibrator");
#endif

  if (!reference_input)
    cerr << "Pulsar::HybridCalibrator::calculate_transformation\n"
            "  WARNING: no reference source input Stokes parameters" << endl;

  if (!reference_observation)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "no reference observation ReferenceCalibrator");

  unsigned nchan;
  if (precalibrator)
    nchan = precalibrator->get_nchan();
  else
    nchan = reference_observation->get_nchan();

  if (reference_input && reference_input->get_nchan() != nchan)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "reference input CalibratorStokes nchan=%d != %d",
		 reference_input->get_nchan(), nchan);
  
  if (verbose > 2)
    cerr << "Pulsar::HybridCalibrator::calculate_transformation"
      " nchan=" << nchan << endl;

  // the calibrator hi and lo levels from the PolnCal archive
  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  reference_observation->get_levels (nchan, cal_hi, cal_lo);

  unsigned npol = cal_hi.size();

  // coherency products in a single channel
  vector<Estimate<double> > cal (npol);

  transformation.resize (nchan);

  // the correction to supplement the PolnCalibrator
  Reference::To<Calibration::SingleAxis> correction;

  // the SingleAxis interpretation of reference signal
  Calibration::SingleAxis pre_single_axis;

  for (unsigned ichan=0; ichan<nchan; ++ichan) try
  {
    if (verbose > 2)
      cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	" ichan=" << ichan << endl;

    if (reference_input && !reference_input->get_valid (ichan))
    {
      if (verbose > 2)
	cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	  " reference input flagged invalid" << endl;
      transformation[ichan] = 0;
      continue;
    }

    if (precalibrator && !precalibrator->get_transformation_valid (ichan))
    {
      if (verbose > 2)
	cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	  " precalibrator flagged invalid" << endl;
      transformation[ichan] = 0;
      continue;
    }

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
      cal_stokes = reference_input->get_stokes(ichan);

    // get the precalibrator transformation
    Jones< Estimate<double> > response (1.0);
    if (precalibrator)
      precalibrator->get_transformation (ichan)->evaluate (response);

    // get the Receiver correction, if any
    {
      const Receiver* receiver;
      BasisCorrection basis_correction;

      // Try using precalibrator's version first
      if (precalibrator && precalibrator->has_Receiver())
        receiver = precalibrator->get_Receiver();

      // Next try the reference obs version
      else if (reference_observation->has_Receiver())
        receiver = reference_observation->get_Receiver();

      response *= basis_correction (receiver);
    }

    // pass the reference Stokes parameters through the instrument
    Stokes< Estimate<double> > input_stokes = transform (cal_stokes, response);

    // solve for the SingleAxis model that relates input and output states
    correction = new Calibration::SingleAxis;

    if (!solver)
      solver = new ::Calibration::SingleAxisSolver;

    solver->set_input (input_stokes);
    solver->set_output (output_stokes);
    solver->solve (correction);

    // produce the supplemented transformation, 

    MEAL::ProductRule<MEAL::Complex2>* result;
    result = new MEAL::ProductRule<MEAL::Complex2>;

    *result *= correction;
    if (precalibrator) 
      *result *= precalibrator->get_transformation (ichan);

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

