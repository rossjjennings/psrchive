//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Multivariate.h

#ifndef __MEAL_Multivariate_H
#define __MEAL_Multivariate_H

#include "MEAL/MultivariatePolicy.h"
#include "Reference.h"

namespace MEAL {

  //! Abstract template base class of multivariate Function implementations
  /*! All abscissae are of type double */
  template<class T>
  class Multivariate : public T 
  {
    unsigned ndim;

  public:

    //! Construct with number of dimensions (independent abscissae)
    Multivariate (unsigned _ndim)
    { ndim = _ndim;
      set_multivariate_policy ( new MultivariatePolicy (ndim, this) ); }

    //! Copy constructor
    Multivariate (const Multivariate& copy) : T (copy) { ndim = copy.ndim; }

    //! Assignment operator
    Multivariate& operator = (const Multivariate& copy);

    //! Clone
    Multivariate<T>* clone () const
    { throw Error (InvalidState, "Multivariate<T>::clone", "not implemented"); }

    //! Set the abscissa value for the specified dimension
    virtual void set_abscissa (unsigned idim, double value)
    { multivariate_policy->set_abscissa (idim, value); }

    //! Get the abscissa value for the specified dimension
    double get_abscissa (unsigned idim) const
    { return multivariate_policy->get_abscissa (idim); }

  protected:

    template <class U>
    friend class FunctionPolicyTraits;
    
    void set_multivariate_policy (MultivariatePolicy* policy)
    { this->argument_policy = multivariate_policy = policy; }

    MultivariatePolicy* get_multivariate_policy () const
    { return multivariate_policy; }

    //! Copy the parameter policy of another instance
    void copy_multivariate_policy (const Multivariate* other)
    { set_multivariate_policy (other->get_multivariate_policy()); }

  private:

    //! The multivariate implementation
    Reference::To<MultivariatePolicy> multivariate_policy;

  };

  template<class T>
  class FunctionPolicyTraits< Multivariate<T> >
  {
  public:
    static void composite_component (Multivariate<T>* composite,
				     Multivariate<T>* component)
    {
      component->copy_multivariate_policy (composite);
    }
  };
}

template<class T>
MEAL::Multivariate<T>& MEAL::Multivariate<T>::operator = (const Multivariate& copy)
{ 
  if (&copy == this)
    return this;

  for (unsigned idim=0; idim < ndim; idim++)
    set_abscissa( idim, copy.get_abscissa(idim) );

  T::operator=( copy );

  return *this;
}

#endif

