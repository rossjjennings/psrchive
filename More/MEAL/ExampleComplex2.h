//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/ExampleComplex2.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Example_H
#define __Example_H

#include "MEPL/OptimizedComplex2.h"

namespace Model {

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
    void calculate (Jones<double>& result, vector<Jones<double> >* gradient);
   
  };

}

#endif
