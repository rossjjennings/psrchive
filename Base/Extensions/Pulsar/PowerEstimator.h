//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/PowerEstimator.h

#ifndef __Pulsar_PowerEstimator_h
#define __Pulsar_PowerEstimator_h

#include "Pulsar/ArchiveExtension.h"
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
