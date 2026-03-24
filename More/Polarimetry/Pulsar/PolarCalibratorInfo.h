//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolarCalibratorInfo.h

#ifndef __PolarCalibratorInfo_H
#define __PolarCalibratorInfo_H

#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/PolnCalibratorInfo.h"

namespace Pulsar {

  //! Communicates PolarCalibrator parameters to plotting routines
  class PolarCalibrator::Info : public PolnCalibrator::Info
  {
    public:

    //! Constructor
    Info (const PolnCalibrator* calibrator);
          
    //! Return the title
    std::string get_title () const;

    //! Return the number of parameter classes
    unsigned get_nclass () const;

    //! Return the name of the specified class
    std::string get_label (unsigned iclass) const;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;

    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
                               unsigned iparam) const;

  };

}

#endif
