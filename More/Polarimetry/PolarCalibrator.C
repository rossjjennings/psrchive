#include "Pulsar/PolarCalibrator.h"

//! Construct from an single PolnCal Pulsar::Archive
Pulsar::PolarCalibrator::PolarCalibrator (const Archive* archive) 
  : ArtificialCalibrator (archive)
{
}

Pulsar::PolarCalibrator::~PolarCalibrator ()
{
}

//! Return the system response as determined by the Polar Transformation
::Calibration::Transformation*
Pulsar::PolarCalibrator::solve (const vector<Estimate<double> >& hi,
				const vector<Estimate<double> >& lo)
{
  if ( hi.size() != 4 || lo.size() != 4 )
    throw Error (InvalidParam, "Pulsar::PolarCalibrator::solve",
		 "hi.size=%d or lo.size=%d != 4", hi.size(), lo.size());

  if (verbose)
    cerr << "Pulsar::PolarCalibrator::solve" << endl;

  // Convert the coherency vectors into Stokes parameters.  
  Stokes< Estimate<double> > stokes_hi = convert (hi);
  Stokes< Estimate<double> > stokes_lo = convert (lo);

  stokes_hi *= 2.0;
  stokes_lo *= 2.0;

  Reference::To<Calibration::Polar> polar = new Calibration::Polar;

  polar->solve (stokes_hi, stokes_lo);

  return polar.release();
}


Pulsar::PolarCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

//! Return the number of parameter classes
unsigned Pulsar::PolarCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
const char* Pulsar::PolarCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "Gain (C\\d0\\u)";
  case 1:
    return "Boost, sinh\\(2128)\\.m\\b\\u \\(0832)";
  case 2:
    return "Rotation";
  default:
    return "";
  }
}
  
//! Return the number of parameters in the specified class
unsigned Pulsar::PolarCalibrator::Info::get_nparam (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return 1;
  case 1:
  case 2:
    return 3;
  default:
    return 0;
  }
}

Pulsar::Calibrator::Info* Pulsar::PolarCalibrator::get_Info () const
{
  return new PolarCalibrator::Info (this);
}
