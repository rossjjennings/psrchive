#include "Pulsar/ReceptionCalibrator.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"

//! Communicates Polar Model parameters
class InstrumentInfo : public Pulsar::SingleAxisCalibrator::Info {

public:

  //! Constructor
  InstrumentInfo (const Pulsar::PolnCalibrator* calibrator) :
    Pulsar::SingleAxisCalibrator::Info (calibrator) {}
      
  //! Return the number of parameter classes
  unsigned get_nclass () const
  {
    return Pulsar::SingleAxisCalibrator::Info::get_nclass() + 2;
  }
  
  //! Return the name of the specified class
  const char* get_name (unsigned iclass) const
  {
    if (iclass < Pulsar::SingleAxisCalibrator::Info::get_nclass())
      return Pulsar::SingleAxisCalibrator::Info::get_name(iclass);

    iclass -= Pulsar::SingleAxisCalibrator::Info::get_nclass();

    switch (iclass) {
    case 0:
      return "Ellipticity";
    case 1:
      return "Orientation";
    default:
      return "";
    }
  }

  //! Return the number of parameters in the specified class
  unsigned get_nparam (unsigned iclass) const
  {
    if (iclass < Pulsar::SingleAxisCalibrator::Info::get_nclass())
      return Pulsar::SingleAxisCalibrator::Info::get_nparam(iclass);

    iclass -= Pulsar::SingleAxisCalibrator::Info::get_nclass();

    if (iclass < 2)
      return 2;

    return 0;
  }

};

Pulsar::Calibrator::Info* Pulsar::ReceptionCalibrator::get_Info () const
{
  switch (model_type) {

  case Calibrator::Hamaker:
    return new PolarCalibrator::Info (this);
  case Calibrator::Britton:
    return new InstrumentInfo (this);
  default:
    return 0;

  }
}

