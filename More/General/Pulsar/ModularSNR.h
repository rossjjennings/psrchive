//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ModularSNR.h

#ifndef __Pulsar_ModularSNR_h
#define __Pulsar_ModularSNR_h

#include "Pulsar/SNRatioEstimator.h"

namespace Pulsar {

  class ProfileWeightFunction;

  //! Calculates the S/N using an on-pulse and off-pulse ProfileWeightFunction
  class ModularSNR : public SNRatioEstimator {

  public:

    //! Default constructor
    ModularSNR ();

    //! Destructor
    ~ModularSNR ();

    //! Set the function used to compute the baseline
    void set_baseline_estimator (ProfileWeightFunction* function);
    ProfileWeightFunction* get_baseline_estimator () const;

    //! Set the function used to find the on-pulse
    void set_onpulse_estimator (ProfileWeightFunction* function);
    ProfileWeightFunction* get_onpulse_estimator () const;

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    ModularSNR* clone () const;

  protected:

    //! The function used to compute the baseline
    Reference::To<ProfileWeightFunction> baseline_estimator;

    //! The function used to find the onpulse region
    Reference::To<ProfileWeightFunction> onpulse_estimator;

  };

}

#endif
