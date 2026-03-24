//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/ManagedStrategies.h

#ifndef __Pulsar_ManagedStrategies_h
#define __Pulsar_ManagedStrategies_h

#include "Pulsar/ProfileStrategies.h"

namespace Pulsar {
    
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
      its strategies are centrally managed.  Therefore, this
      class is just a place-holder.  When Profile::get_strategy
      (defined in More/) is called, this method will check and see
      that its strategy instance is of type ManagedStrategies.  If so,
      it will set its strategy to the one retrieved from its
      Integration container via this class.
  */
  class ManagedStrategies : public Profile::Strategies
  {
    Reference::To<Integration, false> container;
    bool to_clone = false;

  public:

    //! Construct with a pointer to the container
    ManagedStrategies (Integration*);

    //! Copy constructor
    ManagedStrategies (const ManagedStrategies&);

    //! Copy constructor for later cloning
    ManagedStrategies (const ManagedStrategies*);

    //! Destructor
    ~ManagedStrategies ();

    //! Return the container
    Integration* get_container();
    
    //! The implementation of the baseline finding algorithm
    ProfileWeightFunction* baseline () const override;

    //! The implementation of the on-pulse finding algorithm
    ProfileWeightFunction* onpulse () const override;

    //! The implementation of the signal-to-noise ratio calculation
    SNRatioEstimator* snratio () const override;

    //! The implementation of the pulse width estimator
    WidthEstimator* width () const override;

    //! Clone
    ManagedStrategies* clone () const override;

    //! Return true if the container strategy should be cloned
    bool to_be_cloned () const { return to_clone; }
  };
}

#endif
