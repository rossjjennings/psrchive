//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/Correlation.h

#ifndef __Pulsar_CorrelationInfo_H
#define __Pulsar_CorrelationInfo_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  //! Communicates information about correlation between model parameters
  class CorrelationInfo : public Calibrator::Info
  {

  public:

    //! Constructor
    CorrelationInfo (unsigned index, const PolnCalibrator* calibrator);
      
    //! Return the title
    std::string get_title () const override;

    //! Return the number of frequency channels
    unsigned get_nchan () const override;
    
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

  protected:

    //! The PolnCalibrator with correlation information to be plotted
    Reference::To<const PolnCalibrator> poln_calibrator;

    //! Index of the parameter for which correlation information will be plotted
    unsigned index = 0;

    //! Parameter names
    std::vector<std::string> names;
  };

}

#endif
