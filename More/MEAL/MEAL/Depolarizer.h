//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Depolarizer.h,v $
   $Revision: 1.4 $
   $Date: 2008/06/15 17:22:59 $
   $Author: straten $ */

#ifndef __MEAL_Depolarizer_H
#define __MEAL_Depolarizer_H

#include "MEAL/Real4.h"

namespace MEAL
{

  //! Represents a pure depolarizer transformation
  class Depolarizer : public Real4
  {
  public:

    Depolarizer ();

    Depolarizer* clone () const;

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
