//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Convert.h

#ifndef __MEAL_UnaryConvert_H
#define __MEAL_UnaryConvert_H

#include "MEAL/Convert.h"

namespace MEAL {

  //! Convert a function to another type using a unary function

  /*! Both the result of the function and its partial derivatives are
    passed through the unary function */

  template<typename From, typename To, typename Method>
  class UnaryConvert : public Convert<From,To>
  {

  public:

    UnaryConvert (Method m) { method = m; }

    //! Return the name of the class
    std::string get_name () const
    { return "UnaryConvert<" + std::string(From::Name)
	+ "," + std::string(To::Name)+ ">"; }

  protected:

    //! Calculate the Mueller matrix and its gradient
    virtual void calculate (typename To::Result& result,
			    std::vector<typename To::Result>* grad)
    {
      std::vector<typename From::Result> from_grad;
      typename From::Result from_result;

      from_result = this->get_model()->evaluate( (grad) ? &from_grad : 0 );
      result = method (from_result);
      if (!grad)
	return;

      for (unsigned i=0; i<this->get_nparam(); i++)
	(*grad)[i] = method (from_grad[i]);
    }

    //! The Unary function used to complete the conversion
    Method method;
  };

  template<typename T, typename M>
  UnaryConvert<T,T,M>* convert (T* function, M method)
  {
    UnaryConvert<T,T,M>* result = new UnaryConvert<T,T,M> (method);
    result->set_model (function);
    return result;
  }

  template<typename T>
  T* convert (T* function,
	      typename T::Result (*method) (const typename T::Result&))
  {
    typedef typename T::Result (*M) (const typename T::Result&);
    UnaryConvert<T,T,M>* result = new UnaryConvert<T,T,M> (method);
    result->set_model (function);
    return result;
  }

}

#endif
