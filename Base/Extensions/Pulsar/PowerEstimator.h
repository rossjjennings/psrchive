//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PowerEstimator.h,v $
   $Revision: 1.5 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Pulsar_PowerEstimator_h
#define __Pulsar_PowerEstimator_h

#include "Pulsar/Archive.h"
#include <vector>

namespace Pulsar {

  //! Produces on-pulse and off-pulse power spectra
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
