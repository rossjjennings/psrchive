//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gain.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_Gain_H
#define __MEAL_Gain_H

#include "MEAL/OptimizedComplex2.h"

namespace MEAL {

  //! A gain transformation
  class Gain : public OptimizedComplex2 {

  public:

    //! Default constructor
    Gain ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Return the name of the specified parameter
    std::string get_param_name (unsigned index) const;

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
