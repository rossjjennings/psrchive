#include "SingleAxisCalibrator.h"

Pulsar::SingleAxisCalibrator::~SingleAxisCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the SingleAxisModel
Jones<double> 
Pulsar::SingleAxisCalibrator::solve (const vector<Estimate<double> >& hi,
				     const vector<Estimate<double> >& lo,
				     unsigned ichan)
{
  unsigned npol = hi.size();

  vector<Estimate<double> > cal (npol);
  for (unsigned ipol=0; ipol<npol; ++ipol)
    cal[ipol] = hi[ipol] - lo[ipol];

  Calibration::SingleAxisModel qm;
  qm.solve (cal, verbose);

  if (store_parameters)
    model[ichan] = qm;

  return qm.get_Jones();
}

//! Resize the space used to store SingleAxisModel parameters
void Pulsar::SingleAxisCalibrator::resize_parameters (unsigned nchan)
{
  if (verbose)
    cerr << "Pulsar::SingleAxisCalibrator::resize_parameters "<< nchan <<endl;

  model.resize (nchan);
}

 
