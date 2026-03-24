//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
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
    
  public:

    //! Constructor
    BrittonInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the PGPLOT-encoded label of the specified class
    std::string get_label_feed (unsigned iclass) const override;

    //! Return the PGPLOT-encoded name of the specified parameter
    std::string get_param_name_feed (unsigned iparam) const override;

    //! Return the estimate of the specified parameter
    Estimate<float> get_param_feed (unsigned ichan, unsigned iclass, unsigned iparam) const override;

    //! This model has 7 free parameters
    static constexpr unsigned nparam = 7;

  protected:

    //! When true, b_v and r_u are swapped in Equation 19
    bool degeneracy_isolated;

    //! When true, there are data to plot for this parameter
    bool available_data[nparam];
  };

}

#endif
