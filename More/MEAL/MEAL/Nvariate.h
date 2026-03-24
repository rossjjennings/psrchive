//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Nvariate.h

#ifndef __MEAL_Nvariate_H
#define __MEAL_Nvariate_H

namespace MEAL
{
  //! Pure virtual base class of functions that have one or more abscissa(e)
  template<class T>
  class Nvariate: public T
  {
  public:

    virtual Nvariate<T>* clone () const = 0;

    //! Get the number of abscissa (dimension) of the function
    virtual unsigned get_ndim () const = 0;

    //! Set the abscissa value for the specified dimension
    virtual void set_abscissa_value (unsigned idim, double value) = 0;

    //! Get the abscissa value for the specified dimension
    virtual double get_abscissa_value (unsigned idim) const = 0;

  };
}

#endif
