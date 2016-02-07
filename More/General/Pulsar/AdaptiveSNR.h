//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/AdaptiveSNR.h

#ifndef __Pulsar_AdaptiveSNR_h
#define __Pulsar_AdaptiveSNR_h

#include "Pulsar/SNRatioEstimator.h"

namespace Pulsar {

  class ProfileWeightFunction;

  //! Calculates the signal-to-noise ratio using a ProfileWeightFunction
  class AdaptiveSNR : public SNRatioEstimator {

  public:

    //! Default constructor
    AdaptiveSNR ();

    //! Destructor
    ~AdaptiveSNR ();

    //! Set the function used to compute the baseline
    void set_baseline_estimator (ProfileWeightFunction* function);

    ProfileWeightFunction* get_baseline_estimator () const;

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    AdaptiveSNR* clone () const;

  protected:

    //! The function used to compute the baseline
    Reference::To<ProfileWeightFunction> baseline_estimator;

  };

}

#endif
