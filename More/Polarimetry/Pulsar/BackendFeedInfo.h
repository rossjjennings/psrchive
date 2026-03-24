//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BackendFeedInfo.h

#ifndef __Pulsar_BackendFeedInfo_H
#define __Pulsar_BackendFeedInfo_H

#include "Pulsar/SingleAxisCalibratorInfo.h"

namespace Pulsar {

  //! Communicates BackendFeed parameters to plotting routines
  class BackendFeedInfo : public SingleAxisCalibrator::Info {

  public:

    //! Constructor
    BackendFeedInfo (const PolnCalibrator* calibrator);
    
    //! Return the number of parameter classes
    unsigned get_nclass () const override;
    
    //! Return the name of the specified class
    std::string get_label (unsigned iclass) const override;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const override;

    //! Return the PGPLOT-encoded name of the specified parameter
    std::string get_param_name (unsigned iparam) const override;

    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass, unsigned iparam) const override;

    //! Return the PGPLOT-encoded label of the specified class of the feed component
    virtual std::string get_label_feed (unsigned iclass) const = 0;

    //! Return the PGPLOT-encoded name of the specified parameter of the feed component
    virtual std::string get_param_name_feed (unsigned iparam) const = 0;
    
    //! Return the estimate of the specified parameterof the feed component
    virtual Estimate<float> get_param_feed (unsigned ichan, unsigned iclass, unsigned iparam) const = 0;

    int get_colour_index (unsigned iclass, unsigned iparam) const;

  };

}

#endif
