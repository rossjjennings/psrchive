#include "PolarCalibrator.h"

Pulsar::PolarCalibrator::~PolarCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the PolarModel
Jones<double> 
Pulsar::PolarCalibrator::solve (const vector<Estimate<double> >& hi,
				const vector<Estimate<double> >& lo,
				unsigned ichan)
{
  Calibration::PolarModel qm;
  qm.solve (hi, lo);
  
  if (store_parameters)
    model[ichan] = qm;
  
  return qm.get_Jones();
}

//! Resize the space used to store PolarModel parameters
void Pulsar::PolarCalibrator::resize_parameters (unsigned nchan)
{
  if (verbose)
    cerr << "Pulsar::PolarCalibrator::resize_parameters "<< nchan <<endl;
  
  model.resize (nchan);
}

 
