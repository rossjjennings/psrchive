//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluxCalibratorInfo.h

#ifndef __Pulsar_FluxCalibratorInfo_H
#define __Pulsar_FluxCalibratorInfo_H

#include "Pulsar/FluxCalibrator.h"

namespace Pulsar {

  //! FluxCalibrator parameter communication
  class FluxCalibrator::Info : public Calibrator::Info
  {
  public:

    //! Constructor
    Info (const FluxCalibrator* cal) { instance = cal; }
    
    //! Return the title
    std::string get_title () const;

    //! Return the number of frequency channels
    unsigned get_nchan () const;

    //! Return the number of parameter classes
    unsigned get_nclass () const { return 2; }
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;
    
  protected:
    
    Reference::To<const FluxCalibrator> instance;
      
  };

}

#endif
