//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/UnaryStatistic.h

#ifndef __UnaryStatistic_h
#define __UnaryStatistic_h

#include "Identifiable.h"

//! Commmon statistics that can be derived from a single array of floats
class UnaryStatistic : public Identifiable
{
public:

  //! Create a new instance of UnaryStatistic based on name
  static UnaryStatistic* factory (const std::string& name);

  //! Returns a list of available UnaryStatistic children
  static const std::vector<UnaryStatistic*>& children ();

  //! Construct with a name and description
  UnaryStatistic (const std::string& name, const std::string& description);

  //! Derived types define the value returned
  virtual double get (const std::vector<double>&) = 0;

  //! Derived types must also define clone method
  virtual UnaryStatistic* clone () const = 0;

private:

  //! thread-safe build for factory
  static void build (); 
};

#endif

