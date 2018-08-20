//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/ProfileStrategies.h

#ifndef __Pulsar_ProfileStrategies_h
#define __Pulsar_ProfileStrategies_h

#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Manages the strategies that implement algorithms
  class Profile::Strategies : public Reference::Able
  {
  public:
    
    //! The implementation of the baseline finding algorithm
    virtual ProfileWeightFunction* baseline () const = 0;

    //! The implementation of the on-pulse finding algorithm
    virtual ProfileWeightFunction* onpulse () const = 0;

    //! The implementation of the signal-to-noise ratio calculation
    virtual SNRatioEstimator* snratio () const = 0;
  };

  class StrategySet : public Profile::Strategies
  {
    //! The default implementation of the baseline finding algorithm
    mutable Reference::To<ProfileWeightFunction> baseline_strategy;

    //! The default implementation of the onpulse finding algorithm
    mutable Reference::To<ProfileWeightFunction> onpulse_strategy;

    //! The default implementation of the snr method
    mutable Reference::To<SNRatioEstimator> snratio_strategy;

  public:

    //! The default implementation of the baseline finding algorithm
    static Option< Reference::To<ProfileWeightFunction> > default_baseline;

    //! The default implementation of the onpulse finding algorithm
    static Option< Reference::To<ProfileWeightFunction> > default_onpulse;

    //! The default implementation of the snr method
    static Option< Reference::To<SNRatioEstimator> > default_snratio;

    //! The implementation of the baseline finding algorithm
    ProfileWeightFunction* baseline () const;
    void set_baseline (ProfileWeightFunction*);
    
    //! The implementation of the on-pulse finding algorithm
    ProfileWeightFunction* onpulse () const;
    void set_onpulse (ProfileWeightFunction*);

    //! The implementation of the signal-to-noise ratio calculation
    SNRatioEstimator* snratio () const;
    void set_snratio (SNRatioEstimator*);
  };

  class Integration;
  
  //! Inserted by Integration class to tell Profile instance it is managed
  /*! This class addresses the issue raised by the separation of Base/
      and More/ sub-directories.  The end goal is to have each Profile
      use a centralised set of strategies that are managed by the
      Archive class.  This allows things like setting an unique set of
      strategies for an individual Archive instance, such that all of
      the Profile instances that it contains behave in a certain way
      other than the default.  For example, when the Archive contains
      an observation of a noise diode, it can set the baseline and onpulse
      estimation strategies to algorithms that are specific to square waves.
      All Profile instances contained by that Archive will then use those
      strategies.  

      More fundamentally, the problem that this class is attempting to
      solve is failure to apply the Interface Segregation Principle
      when designing the Profile class.  That is, the Profile class is
      both a container and a set of algorithms (baseline estimation,
      signal-to-noise ratio calculation, etc.) that are applied to the
      contents of the container.  In principle, the containerly nature
      of the Profile class is implemented by methods that are defined
      in Base/ and the algorithmic nature of the Profile class is 
      implemented by methods that are defined in More/.  This allows
      an end-user program to be compiled that uses only the container
      and needs to be linked only against the library defined in Base/;
      within psrchive, psredit is an example of such a program.

      Now, ProfileWeightFunction and SNRatioEstimator are defined in
      More/ and therefore an object that is created in Base/ could
      never create an instance of ProfileWeightFunction or
      SNRatioEstimator, as this would create a dependence on the
      library defined in More/.  However, it is necessary for the
      Integration class to inform each of its Profile instances that
      its strategies are being centrally managed.  Therefore, this
      class is just a place-holder.  When Profile::get_strategy
      (defined in More/) is called, this method will check and see
      that its strategy instance is of type ManagedStrategies.  If so,
      it will set its strategy to the one retrieved from its
      Integration container via this class.
  */
  class ManagedStrategies : public Profile::Strategies
  {
    Reference::To<Integration, false> container;
    
  public:

    //! Construct with a pointer to the container
    ManagedStrategies (Integration*);

    //! Return the container
    Integration* get_container();
    
    //! The implementation of the baseline finding algorithm
    ProfileWeightFunction* baseline () const;

    //! The implementation of the on-pulse finding algorithm
    ProfileWeightFunction* onpulse () const;

    //! The implementation of the signal-to-noise ratio calculation
    SNRatioEstimator* snratio () const;
  };
}

#endif
