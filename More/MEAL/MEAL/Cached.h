//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Cached.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:24:36 $
   $Author: straten $ */

#ifndef __Cached_H
#define __Cached_H

#include "MEAL/CalculatePolicy.h"
#include <iostream>

namespace MEAL {
  
  //! This evaluation policy uses previously cached results when possible
  /*! This class re-computes the cached values of the result and
    gradient only if the context has been altered. */

  template<class T>
  class Cached : public CalculatePolicy<T>  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    Cached (T* context) : CalculatePolicy<T> (context)
    { gradient_is_cached = false; }

    //! Implement the evaluate method of the Function
    Result evaluate (std::vector<Result>* gradient = 0) const
    {
      if (Function::very_verbose)
	std::cerr << class_name() + "evaluate" << std::endl;

      update (gradient != 0);
  
      if (gradient) {
	gradient->resize (cached_gradient.size());
	for (unsigned i=0; i<cached_gradient.size(); i++)
	  (*gradient)[i] = cached_gradient[i];
      }

      return cached_result;
    }

    //! Return the name of the class for debugging
    std::string class_name() const
    { return "MEAL::Cached[" + get_context()->get_name() + "]::"; }

  protected:

    //! Updates the result (and gradient when needed)
    void update (bool need_gradient) const
    {
      const_cast<Cached*>(this)->update (need_gradient);
    }

    void update (bool need_gradient)
    {
      if (! (get_context()->get_evaluation_changed()
	     || (need_gradient && !gradient_is_cached))) {
	if (Function::very_verbose) 
	  std::cerr << class_name() + "update no change" << std::endl;
	return;
      }

      std::vector<Result>* grad_ptr = 0;

      if (need_gradient) {
	cached_gradient.resize (get_context()->get_nparam());
	grad_ptr = &cached_gradient;
	gradient_is_cached = true;  // well, it will be after calling calculate
      }

      calculate (cached_result, grad_ptr);

      get_context()->set_evaluation_changed (false);
    }

    //! The result cached in the last call to calculate
    Result cached_result;

    //! The gradient cached in the last call to calculate
    std::vector<Result> cached_gradient;

  private:

    //! Flag set when gradient has been computed
    bool gradient_is_cached;

  };

}

#endif
