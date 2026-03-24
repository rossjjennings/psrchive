//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SingleAxisCalibrator.h

#ifndef __Pulsar_SingleAxisCalibratorInfo_H
#define __Pulsar_SingleAxisCalibratorInfo_H

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolnCalibratorInfo.h"

namespace Pulsar {

  //! Communicates SingleAxisCalibrator parameters
  class SingleAxisCalibrator::Info : public PolnCalibrator::Info
  {

  public:

    //! Constructor
    Info (const PolnCalibrator* calibrator);

    //! Return the number of parameters
    unsigned get_nparam () const { return 3; }

    //! Return the title
    std::string get_title () const override;

    //! Return the number of parameter classes
    unsigned get_nclass () const override;

    //! Return the PGPLOT-encoded label of the specified class
    std::string get_label (unsigned iclass) const override;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const override;

    //! Return the PGPLOT-encoded name of the specified parameter
    std::string get_param_name (unsigned iparam) const override;

    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, 
                               unsigned iclass,
                               unsigned iparam) const override;

  };

}

#endif
