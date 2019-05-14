//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/IXRInfo.h

#ifndef __Pulsar_IXRInfo_H
#define __Pulsar_IXRInfo_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  //! Communicates PolnCalibrator IXR to plotting routines
  class IXRInfo : public Calibrator::Info 
  {

  public:

    //! Constructor
    IXRInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the number of frequency channels
    unsigned get_nchan () const;
    
    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;

    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

  protected:

    //! The PolnCalibrator to be plotted
    Reference::To<const PolnCalibrator> poln_calibrator;

  };

}

#endif
