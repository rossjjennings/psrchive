//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarArgument.h,v $
   $Revision: 1.5 $
   $Date: 2005/04/06 20:23:36 $
   $Author: straten $ */

#ifndef __ScalarArgument_H
#define __ScalarArgument_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"

namespace MEAL {

  //! Represents a scalar argument
  class ScalarArgument : public Univariate<Scalar> {

  public:

    //! Default constructor
    ScalarArgument ();

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the argument
    void calculate (double& result, std::vector<double>* gradient=0);
    
  };

}

#endif
