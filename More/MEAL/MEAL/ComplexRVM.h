//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ComplexRVM.h,v $
   $Revision: 1.1 $
   $Date: 2009/06/10 10:56:05 $
   $Author: straten $ */

#ifndef __ComplexRVM_H
#define __ComplexRVM_H

#include "MEAL/Complex.h"
#include "MEAL/ProductRule.h"

namespace MEAL {

  class RotatingVectorModel;

  //! Rotating Vector Model of Stokes Q and U as a function of pulse phase
  class ComplexRVM : public ProductRule<Complex>
  {

  public:

    //! Default constructor
    ComplexRVM ();

    //! Copy constructor
    ComplexRVM (const ComplexRVM& copy);

    //! Assignment operator
    ComplexRVM& operator = (const ComplexRVM& copy);

    //! Destructor
    ~ComplexRVM ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Return the rotating vector model
    RotatingVectorModel* get_rvm ();

  private:

    void calculate (Result&, std::vector<Result>*);
    void init ();

    Reference::To<RotatingVectorModel> rvm;

  };

}

#endif
