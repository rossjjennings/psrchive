//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Argument.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Calibration_Argument_Header
#define __Calibration_Argument_Header

#include "Reference.h"

namespace Calibration {

  //! Pure virtual base class of function arguments
  /*! The Argument class represents an independent variable of a
    function.  The values taken by this variable are represented by
    Argument::Value instances.  Nothing is known about the type of the
    variable, enabling Model classes to be developed with an arbitrary
    number of independent variables of arbitrary type. */

  class Argument : public Reference::Able {

    public:

    //! Verbosity flag
    static bool verbose;
    
    //! Destructor
    virtual ~Argument ();
    
    //! The value of an argument
    class Value : public Reference::Able {
      
    public:
      
      //! Destructor
      virtual ~Value ();
      
      //! Apply the value of this Argument
      virtual void apply () const = 0;
      
    };

  };

}

#endif
