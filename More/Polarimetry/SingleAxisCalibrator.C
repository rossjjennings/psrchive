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

//! SingleAxisCalibrator parameter communication
class SingleAxisCalibratorInfo : public Pulsar::Calibrator::Info {

public:
  //! Constructor
  SingleAxisCalibratorInfo (const Pulsar::SingleAxisCalibrator* cal) 
  { calibrator = cal; }

  //! Return the number of parameter classes
  unsigned get_nclass () const { return 3; }

  //! Return the name of the specified class
  const char* get_name (unsigned iclass)
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
  unsigned get_nparam (unsigned iclass)
  {
    return 1;
  }
  
  //! Return the estimate of the specified parameter
  Estimate<float> get_param (unsigned ichan, unsigned iclass,
			     unsigned iparam)
  {
    return calibrator->get_Transformation(ichan)->get_Estimate(iclass);
  }
  
protected:

  Reference::To<const Pulsar::SingleAxisCalibrator> calibrator;

};

Pulsar::Calibrator::Info* Pulsar::SingleAxisCalibrator::get_Info () const
{
  return new SingleAxisCalibratorInfo (this);
}
