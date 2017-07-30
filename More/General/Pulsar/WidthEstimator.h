//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/WidthEstimator.h

#ifndef __Pulsar_WidthEstimator_h
#define __Pulsar_WidthEstimator_h

#include "Pulsar/Algorithm.h"
#include "TextInterface.h"
#include "PhaseRange.h"

namespace Pulsar {

  class Profile;

  //! Algorithms that estimate the width of the pulse profile
  /*! This pure virtual base class of PhaseWeight algorithms defines the
      interface by which various routines, such as baseline estimators
      can be accessed */
  class WidthEstimator : public Algorithm
  {
  protected:

    //! Derived classes must implement this method
    virtual double get_width_turns (const Profile* profile) = 0;

  public:

    //! Return the width of the pulse profile in turns
    virtual Phase::Value get_width (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Return a copy constructed instance of self
    virtual WidthEstimator* clone () const = 0;

    //! Construct a new WidthEstimator from a string
    static WidthEstimator* factory (const std::string& name_and_parse);

  };

}


#endif // !defined __Pulsar_WidthEstimator_h
