//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/ExampleComplex2.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

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
