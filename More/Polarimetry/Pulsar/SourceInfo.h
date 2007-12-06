//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SourceInfo.h,v $
   $Revision: 1.1 $
   $Date: 2007/12/06 05:49:23 $
   $Author: straten $ */

#ifndef __Pulsar_SourceInfo_H
#define __Pulsar_SourceInfo_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class ReceptionCalibrator;

  //! Communicates Calibrator Stokes parameters
  class SourceInfo : public Calibrator::Info {

  public:

    //! Constructor
    SourceInfo (const ReceptionCalibrator* calibrator, unsigned source_index);
    
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

    //! The ReceptionCalibrator containing the source model
    Reference::To<const ReceptionCalibrator> calibrator;

    //! The index of the source model to be plotted
    unsigned source_index;

    //! Plot all of the Stokes parameters in one panel
    bool together;

  };

}

#endif
