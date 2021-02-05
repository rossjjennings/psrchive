//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseWeightStatistic.h

#ifndef __PhaseWeightStatistic_h
#define __PhaseWeightStatistic_h

#include "UnaryStatistic.h"

namespace Pulsar {

  class PhaseWeight;

  //! Commmon statistics that can be derived from a pulse profile
  class PhaseWeightStatistic : public Identifiable::Decorator
  {
  public:

    //! Create a new instance of PhaseWeightStatistic based on name
    static PhaseWeightStatistic* factory (const std::string& name);

    //! Returns a list of available PhaseWeightStatistic children
    static const std::vector<PhaseWeightStatistic*>& children ();

    //! Construct from a UnaryStatistic
    PhaseWeightStatistic (UnaryStatistic*);

    //! Derived types define the value returned
    virtual double get (const PhaseWeight*);

    //! Derived types must also define clone method
    virtual PhaseWeightStatistic* clone () const;

  private:

    Reference::To<UnaryStatistic> stat;

    //! thread-safe build for factory
    static void build (); 
  };

}

#endif

