//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PowerEstimator.h,v $
   $Revision: 1.4 $
   $Date: 2005/03/22 06:13:26 $
   $Author: straten $ */

#ifndef __Pulsar_PowerEstimator_h
#define __Pulsar_PowerEstimator_h

#include "Pulsar/Archive.h"
#include <vector>

namespace Pulsar {

  //! Pure virtual base class of power estimation Archive::Extension classes
  class PowerEstimator : public Archive::Extension {

  public:

    //! Default constructor
    PowerEstimator (const char* name) : Archive::Extension (name) { }

    //! Return the estimated power spectrum
    virtual void get_power (unsigned isub, unsigned ipol,
			    std::vector<float>& on,
			    std::vector<float>& off) const = 0;

  };

}

#endif
