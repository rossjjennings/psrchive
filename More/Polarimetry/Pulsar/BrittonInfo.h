//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BrittonInfo.h

#ifndef __Pulsar_BrittonInfo_H
#define __Pulsar_BrittonInfo_H

#include "Pulsar/BackendFeedInfo.h"

namespace Pulsar {

  //! Communicates Britton (2000) parameters to plotting routines
  class BrittonInfo : public BackendFeedInfo {

    //! When true, b_v and r_u are swapped in Equation 19
    bool degeneracy_isolated;
    bool constant_orientation;
    
  public:

    //! Constructor
    BrittonInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the name of the specified class
    std::string get_name_feed (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param_feed (unsigned ichan, unsigned iclass,
				    unsigned iparam) const;
  };

}

#endif
