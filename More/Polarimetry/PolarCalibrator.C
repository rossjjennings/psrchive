#include "PolarCalibrator.h"

Pulsar::PolarCalibrator::~PolarCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // destructor can delete forward declared objects
}

//! Return the system response as determined by the PolarModel
Jones<double> 
Pulsar::PolarCalibrator::solve (const vector<Estimate<double> >& hi,
				const vector<Estimate<double> >& lo,
				unsigned ichan)
{
  if ( hi.size () != 4 || hi.size() != lo.size() )
    throw Error (InvalidParam, "Pulsar::PolarCalibrator::solve",
		 "invalid dimension=%d", hi.size());

  // Convert the coherency vectors into Stokes parameters.  
  Stokes< Estimate<double> > stokes_hi = convert (hi);
  Stokes< Estimate<double> > stokes_lo = convert (lo);

  Calibration::Polar qm;
  qm.solve (stokes_hi, stokes_lo);

  // Intensity of off-pulse (system + sky), in CAL flux units
  baseline[ichan] = stokes_lo.s0 / stokes_hi.s0;

  if (store_parameters)
    model[ichan] = qm;
  
  return qm.evaluate();
}

//! Resize the space used to store PolarModel parameters
void Pulsar::PolarCalibrator::resize_parameters (unsigned nchan)
{
  if (verbose)
    cerr << "Pulsar::PolarCalibrator::resize_parameters "<< nchan <<endl;
  
  model.resize (nchan);
}

 
