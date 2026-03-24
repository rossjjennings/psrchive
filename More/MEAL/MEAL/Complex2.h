//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Complex2.h

#ifndef __MEAL_Complex2_H
#define __MEAL_Complex2_H

/*
 * It is necessary to include Jones.h first in order to define true_math::finite(Jones)
 * before the Evaluable::evaluate method is encountered
 */

#include "Jones.h"
#include "MEAL/Complex.h"

template<typename T>
struct EstimateTraits< Jones<T> >
{
  typedef Jones< Estimate<T> > type;
};

template<typename T>
void add_variance (Jones< Estimate<T> >& result,
		   double var, const Jones<T>& grad)
{
  for (unsigned i=0; i < 4; i++)
    add_variance (result[i], var, grad[i]);
}

namespace MEAL
{
  //! Pure virtual base class of all complex 2x2 matrix functions
  class Complex2 : public Evaluable< Jones<double> >
  {
  public:
    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Complex2* clone () const;
  };
}

#endif
