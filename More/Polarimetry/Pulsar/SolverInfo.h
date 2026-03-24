//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SolverInfo.h

#ifndef __Pulsar_SolverInfo_H
#define __Pulsar_SolverInfo_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  class CalibratorStokes;

  //! Communicates PolnCalibrator solver parameters to plotting routines
  class SolverInfo : public Calibrator::Info 
  {

  public:

    //! Constructor
    SolverInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const override;

    //! Return the number of frequency channels
    unsigned get_nchan () const override;
    
    //! Return the number of parameter classes
    unsigned get_nclass () const override;
    
    //! Return the name of the specified class
    std::string get_label (unsigned iclass) const override;

    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const override;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass, unsigned iparam) const override;

  protected:

    //! The PolnCalibrator with solver parameters to be plotted
    Reference::To<const PolnCalibrator> poln_calibrator;

  };

}

#endif
