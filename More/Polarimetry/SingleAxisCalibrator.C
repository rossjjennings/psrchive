#include "Pulsar/SingleAxisCalibrator.h"
#include "Estimate.h"

//! Construct from an single PolnCal Pulsar::Archive
Pulsar::SingleAxisCalibrator::SingleAxisCalibrator (const Archive* archive) 
  : ArtificialCalibrator (archive)
{
}

Pulsar::SingleAxisCalibrator::~SingleAxisCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the SingleAxis
::Calibration::Transformation*
Pulsar::SingleAxisCalibrator::solve (const vector<Estimate<double> >& hi,
				     const vector<Estimate<double> >& lo)
{
  unsigned npol = hi.size();

  vector<Estimate<double> > cal (npol);
  for (unsigned ipol=0; ipol<npol; ++ipol) {
    cal[ipol] = hi[ipol];
    cal[ipol] -= lo[ipol];
  }

  Reference::To<Calibration::SingleAxis> model = new Calibration::SingleAxis;
  model->solve (cal);

  return model.release();
}




Pulsar::SingleAxisCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

//! Return the number of parameter classes
unsigned Pulsar::SingleAxisCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
const char* Pulsar::SingleAxisCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "Gain (C\\d0\\u)";
  case 1:
    return "Diff. Gain";
  case 2:
    return "Diff. Phase";
  default:
    return "";
  }
} 
  
//! Return the number of parameters in the specified class
unsigned Pulsar::SingleAxisCalibrator::Info::get_nparam (unsigned iclass) const
{
  if (iclass < 3)
    return 1;
  return 0;
}

Pulsar::Calibrator::Info* Pulsar::SingleAxisCalibrator::get_Info () const
{
  return new SingleAxisCalibrator::Info (this);
}

Pulsar::Calibrator::Type Pulsar::SingleAxisCalibrator::get_type () const
{
  return SingleAxis;
}
