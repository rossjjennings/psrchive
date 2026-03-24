//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/InstrumentInfo.h

#ifndef __Pulsar_InstrumentInfo_H
#define __Pulsar_InstrumentInfo_H

#include "Pulsar/BackendFeedInfo.h"
#include "Pulsar/Instrument.h"

namespace Pulsar {

  //! Communicates Instrument parameters to plotting routines
  class InstrumentInfo : public BackendFeedInfo {

  public:

    //! Constructor
    InstrumentInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the PGPLOT-encoded label of the specified class
    std::string get_label_feed (unsigned iclass) const override;

    //! Return the PGPLOT-encoded name of the specified parameter
    std::string get_param_name_feed (unsigned iparam) const override;

    //! Return the estimate of the specified parameter
    Estimate<float> get_param_feed (unsigned ichan, unsigned iclass, unsigned iparam) const override;

  protected:

    //! set true when the orientation of receptor 0 is set to zero
    bool fixed_orientation;

  };

}

#endif
