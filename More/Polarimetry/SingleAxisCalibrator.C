#include "SingleAxisCalibrator.h"
#include "Estimate.h"

Pulsar::SingleAxisCalibrator::~SingleAxisCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the SingleAxis
Jones<double> 
Pulsar::SingleAxisCalibrator::solve (const vector<Estimate<double> >& hi,
				     const vector<Estimate<double> >& lo,
				     unsigned ichan)
{
  unsigned npol = hi.size();

  vector<Estimate<double> > cal (npol);
  for (unsigned ipol=0; ipol<npol; ++ipol) {
    cal[ipol] = hi[ipol];
    cal[ipol] -= lo[ipol];
  }

  Calibration::SingleAxis qm;
  qm.solve (cal);

  if (store_parameters)
    model[ichan] = qm;

  return qm.evaluate();
}

//! Resize the space used to store SingleAxis parameters
void Pulsar::SingleAxisCalibrator::resize_parameters (unsigned nchan)
{
  if (verbose)
    cerr << "Pulsar::SingleAxisCalibrator::resize_parameters "<< nchan <<endl;

  model.resize (nchan);
}

 
