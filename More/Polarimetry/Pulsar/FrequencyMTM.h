//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FrequencyMTM.h

#ifndef __Pulsar_FrequencyMTM_H
#define __Pulsar_FrequencyMTM_H

#include "Pulsar/PulsarCalibrator.h"

namespace Pulsar
{

  //! Implements spectral Matrix Template Matching
  /*! This class determines the frequency-dependent system response 
    using a pulsar as a polarized reference source.  The inputs are a 
    well-calibrated (ideally high signal-to-noise-ratio) observation of 
    a pulsar to be used as the reference source and at least one observation of
    the same source. The output is an estimate of the instrumental
    response at the epoch of the observation(s) that are fit to the
    reference source. */
  class FrequencyMTM : public PulsarCalibrator 
  {
    
  public:
    
    //! Constructor
    FrequencyMTM (Calibrator::Type* = 0);

  protected:

  };

}

#endif
