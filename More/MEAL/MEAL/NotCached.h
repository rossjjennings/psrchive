//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NotCached.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:24:36 $
   $Author: straten $ */

#ifndef __NotCached_H
#define __NotCached_H

#include "MEAL/CalculatePolicy.h"

namespace MEAL {
  
  //! This evaluation policy does not use any cache
  /*! This policy always calls the context T::calculate method */
  template<class T>
  class NotCached : public CalculatePolicy<T>  {

  public:

    typedef typename T::Result Result;

    NotCached (T* context) : CalculatePolicy<T> (context) { }

    //! Implement the evaluate method of the Function
    Result evaluate (std::vector<Result>* grad) const
    { Result r; calculate (r, grad); return r; }

  };

}

#endif
