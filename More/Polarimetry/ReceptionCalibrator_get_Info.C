#include "Pulsar/ReceptionCalibrator.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"

namespace Pulsar {

  //! Communicates Polar Model parameters
  class InstrumentInfo : public SingleAxisCalibrator::Info {
    
  public:
    
    //! Constructor
    InstrumentInfo (const PolnCalibrator* calibrator) :
      SingleAxisCalibrator::Info (calibrator) {}
    
    //! Return the number of parameter classes
    unsigned get_nclass () const
    {
      return SingleAxisCalibrator::Info::get_nclass() + 2;
    }
    
    //! Return the name of the specified class
    const char* get_name (unsigned iclass) const
    {
      if (iclass < SingleAxisCalibrator::Info::get_nclass())
	return SingleAxisCalibrator::Info::get_name(iclass);
      
      iclass -= SingleAxisCalibrator::Info::get_nclass();
      
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
      if (iclass < SingleAxisCalibrator::Info::get_nclass())
	return SingleAxisCalibrator::Info::get_nparam(iclass);
      
      iclass -= SingleAxisCalibrator::Info::get_nclass();
      
      if (iclass < 2)
	return 2;
      
      return 0;
    }
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const
    {

      if (iclass < SingleAxisCalibrator::Info::get_nclass())
	return SingleAxisCalibrator::Info::get_param (ichan, iclass, iparam);

      // unscramble the orientation and ellipticity

      iclass -= SingleAxisCalibrator::Info::get_nclass();
      iparam += SingleAxisCalibrator::Info::get_nclass();

      return SingleAxisCalibrator::Info::get_param (ichan, iparam, iclass);

    }
    
  };

}

Pulsar::Calibrator::Info* Pulsar::ReceptionCalibrator::get_Info () const
{
  const_cast<ReceptionCalibrator*>(this)->initialize();

  switch (model_type) {
    
  case Calibrator::Hamaker:
    return new PolarCalibrator::Info (this);
  case Calibrator::Britton:
    return new InstrumentInfo (this);
  default:
    return 0;
    
  }
}

