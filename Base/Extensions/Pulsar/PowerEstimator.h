//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PowerEstimator.h,v $
   $Revision: 1.1 $
   $Date: 2005/02/18 09:04:07 $
   $Author: straten $ */

#ifndef __Pulsar_PowerEstimator_h
#define __Pulsar_PowerEstimator_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Pure virtual base class of power estimation Archive::Extension classes
  class PowerEstimator : public Archive::Extension {

  public:

    //! Return the estimated power
    virtual float get_power (const Archive* archive,
			     unsigned subint, unsigned pol, unsigned chan) = 0;

  };

}

#endif
