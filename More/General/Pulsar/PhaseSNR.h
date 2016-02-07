//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseSNR.h

#ifndef __Pulsar_PhaseSNR_h
#define __Pulsar_PhaseSNR_h

#include "Pulsar/SNRatioEstimator.h"
#include <vector>

namespace Pulsar {

  //! Calculates the signal-to-noise ratio using old default algorithm
  /*!
    This default implementation of the Profile::snr method is taken
    from the old timer archive code.  Using Profile::find_min_phase and
    Profile::find_peak_edges, this function finds the integrated power in
    the pulse profile and divides this by the noise in the baseline.
  */
  class PhaseSNR : public SNRatioEstimator
  {

  public:
    
    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    PhaseSNR* clone () const;

  };

}

#endif

