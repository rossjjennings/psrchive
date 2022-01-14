//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/BinaryStatistic.h

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
  /*! data1 and data2 must have been normalized such that a 
    robust estimate of their variance is equal to unity */
  virtual double get (const std::vector<double>& data1,
		      const std::vector<double>& data2) = 0;

  //! Derived types must also define clone method
  virtual BinaryStatistic* clone () const = 0;

  //! Set the file to which auxiliary data will be printed
  void set_file (FILE* f) { fptr = f; }

protected:

  //! The file to which auxiliary data will be printed
  FILE* fptr;

private:
  
  //! thread-safe build for factory
  static void build (); 
};

#endif

