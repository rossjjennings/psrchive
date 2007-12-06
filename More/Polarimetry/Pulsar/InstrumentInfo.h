//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/InstrumentInfo.h,v $
   $Revision: 1.2 $
   $Date: 2007/12/06 19:58:23 $
   $Author: straten $ */

#ifndef __Pulsar_InstrumentInfo_H
#define __Pulsar_InstrumentInfo_H

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/Instrument.h"

namespace Pulsar {

  //! Communicates Instrument parameters to plotting routines
  class InstrumentInfo : public SingleAxisCalibrator::Info {

  public:

    //! Constructor
    InstrumentInfo (const PolnCalibrator* calibrator);
    
    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

    int get_colour_index (unsigned iclass, unsigned iparam) const;

  protected:

    //! set true when the orientation of receptor 0 is set to zero
    bool fixed_orientation;

    //! Set to the first valid Instrument instance found in PolnCalibrator
    const Calibration::Instrument* instrument;

  };

}

#endif
