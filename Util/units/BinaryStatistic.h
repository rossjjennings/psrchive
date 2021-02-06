//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/BinaryStatistic.h

#ifndef __BinaryStatistic_h
#define __BinaryStatistic_h

#include "Identifiable.h"

//! Commmon statistics that can be derived from two arrays of floats
class BinaryStatistic : public Identifiable
{
public:

  //! Create a new instance of BinaryStatistic based on name
  static BinaryStatistic* factory (const std::string& name);

  //! Returns a list of available BinaryStatistic children
  static const std::vector<BinaryStatistic*>& children ();

  //! Construct with a name and description
  BinaryStatistic (const std::string& name, const std::string& description);

  //! Derived types define the value returned
  virtual double get (const std::vector<double>&, const std::vector<double>&) = 0;

  //! Derived types must also define clone method
  virtual BinaryStatistic* clone () const = 0;

private:

  //! thread-safe build for factory
  static void build (); 
};

#endif

