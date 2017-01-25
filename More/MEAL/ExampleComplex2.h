//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/ExampleComplex2.h

#ifndef __Example_H
#define __Example_H

#include "MEAL/OptimizedComplex2.h"

namespace MEAL {

  //! A gain transformation
  class Example : public OptimizedComplex2 {

  public:

    //! Default constructor
    Example ();

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedComplex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >* gradient);
   
  };

}

#endif
