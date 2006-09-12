//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Depolarizer.h,v $
   $Revision: 1.2 $
   $Date: 2006/09/12 08:09:13 $
   $Author: straten $ */

#ifndef __MEAL_Depolarizer_H
#define __MEAL_Depolarizer_H

#include "MEAL/Real4.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a pure depolarizer transformation
  class Depolarizer : public Real4 {

  public:

    Depolarizer ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Matrix<4,4,double>& result,
		    std::vector<Matrix<4,4,double> >*);

  };

}

#endif
