//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/VariationInfo.h,v $
   $Revision: 1.2 $
   $Date: 2007/12/06 19:23:38 $
   $Author: straten $ */

#ifndef __Pulsar_VariationInfo_H
#define __Pulsar_VariationInfo_H

#include "Pulsar/ReceptionCalibrator.h"
#include "MEAL/Scalar.h"

namespace Pulsar {

  //! Communicates time variation parameters to plotting routines
  class VariationInfo : public Calibrator::Info {

  public:

    //! Things that vary in a ReceptionCalibrator
    enum Which {
      //! Absolute gain
      Gain,
      //! Differential gain
      Boost,
      //! Differential phase
      Rotation
    };

    //! Construct with which function to plot
    VariationInfo (const ReceptionCalibrator* calibrator, Which which);
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the Scalar variation function for the specified channel
    const MEAL::Scalar* get_Scalar (unsigned ichan) const;

  protected:

    //! The ReceptionCalibrator containing the step functions
    Reference::To<const ReceptionCalibrator> calibrator;

    //! Which function to plot
    Which which;

  };

}

#endif
