#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/ArtificialCalibrator.h"
#include "Pulsar/CalibratorStokes.h"

#include "Calibration/SingleAxis.h"

Pulsar::HybridCalibrator::HybridCalibrator (PolnCalibrator* _calibrator)
{
  precalibrator = _calibrator;
  calibrator = precalibrator->get_Archive();
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

//! Set the Stokes parameters of the reference signal
void Pulsar::HybridCalibrator::set_reference_input (CalibratorStokes* reference)
{
  reference_input = reference;
}

//! Set the ArtificialCalibrator data from which to derive a SingleAxis
void Pulsar::HybridCalibrator::set_reference_observation (ArtificialCalibrator* observation)
{
  reference_observation = observation;
}
    
//! Set the PolnCalibrator to be supplemented
void Pulsar::HybridCalibrator::set_precalibrator (PolnCalibrator* _calibrator)
{
  precalibrator = _calibrator;
  calibrator = precalibrator->get_Archive();
}

void Pulsar::HybridCalibrator::calculate_transformation ()
{
  if (!precalibrator)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "no precalibrator PolnCalibrator");

  if (!reference_input)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "no reference input CalibratorStokes");

  if (!reference_observation)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "no reference observation ArtificialCalibrator");

  unsigned nchan = precalibrator->get_Transformation_nchan();

  if (reference_input->get_nchan() != nchan)
    throw Error (InvalidState,
		 "Pulsar::HybridCalibrator::calculate_transformation",
		 "reference input CalibratorStokes nchan=%d != %d",
		 reference_input->get_nchan(), nchan);
  
  if (verbose) cerr << "Pulsar::HybridCalibrator::calculate_transformation"
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

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    if (verbose)
      cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	" ichan=" << ichan << endl;

    if (!reference_input->get_valid (ichan)) {
      if (verbose)
	cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	  " invalid reference input" << endl;
      transformation[ichan] = 0;
      continue;
    }

    if (!precalibrator->get_Transformation_valid (ichan)) {
      if (verbose)
	cerr << "Pulsar::HybridCalibrator::calculate_transformation"
	  " invalid precalibrator" << endl;
      transformation[ichan] = 0;
      continue;
    }

    // S"  are the observed calibrator Stokes parameters
    // S'  are the modeled calibrator Stokes parameters after precalibrator transformation
    // S   are the ideal calibrator Stokes parameters, S=[1,0,1,0]
    //
    // M_1 is the SingleAxis Mueller matrix that converts observed to ideal
    // M_0 is the SingleAxis Mueller matrix that converts modeled to ideal
    //
    // 1)  S" = M_1 S
    // 2)  S' = M_0 S
    // 3)  S" = M_1 M_0^-1 S' = M_s S'
    //
    // This method calculates M_s = M_1 M_0^-1 S'

    // 1) solve S" = M_1 S

    for (unsigned ipol=0; ipol<npol; ++ipol)
      cal[ipol] = cal_hi[ipol][ichan] - cal_lo[ipol][ichan];
    
    correction = new Calibration::SingleAxis;
    correction->solve (cal);

    // 2) solve S' = M_0 S
 
    // pass the input Stokes parameters through the precalibrator transformation

    Calibration::Transformation* xform;
    xform = precalibrator->get_Transformation (ichan);

    Jones< Estimate<double> > response;
    xform->evaluate (response);

    Stokes< Estimate<float> > stokes = reference_input->get_stokes (ichan);
    stokes = response * stokes * herm(response);

    Jones< Estimate<float> > coherence = convert (stokes);

    cal[0] = 0.5 * coherence.j(0,0).real();
    cal[1] = 0.5 * coherence.j(1,1).real();
    cal[2] = 0.5 * coherence.j(1,0).real();
    cal[3] = 0.5 * coherence.j(1,0).imag();

    pre_single_axis.solve (cal);

    // 3) calculate M_s = M_1 M_0^-1

    pre_single_axis.invert ();
    *correction *= pre_single_axis;

    // 4) produce the supplemented transformation, M_s M_B

    Calibration::ProductTransformation* result;
    result = new Calibration::ProductTransformation;

    result->add_Transformation (correction);
    result->add_Transformation (xform);

    transformation[ichan] = result;

  }

  if (verbose)
    cerr << "Pulsar::HybridCalibrator::calculate_transformation exit"
	 << endl;
}

