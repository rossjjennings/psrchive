//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScalarVector.h

#ifndef __ScalarVector_H
#define __ScalarVector_H

#include "MEAL/Scalar.h"

namespace MEAL {

  //! Scalar functions that depend on an index
  /*! This base class represents Scalar functions for which the evaluate
    method depends on an unsigned integer index. */
  class ScalarVector : public Scalar
  {
  public:

    //! Default contructor
    ScalarVector ();

    //! Get the size of the position angle array
    virtual unsigned size () const = 0;

    //! Set the index of the position angle array
    void set_index (unsigned);

    //! Get the index of the position angle array
    unsigned get_index () const;

  protected:

    //! The current index
    unsigned index;

  };

}

#endif
