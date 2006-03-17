//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NotCached.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/17 13:35:25 $
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
