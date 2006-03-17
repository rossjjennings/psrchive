//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gain.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/17 13:35:25 $
   $Author: straten $ */

#ifndef __MEAL_Gain_H
#define __MEAL_Gain_H

#include "MEAL/Complex2.h"

namespace MEAL {

  //! A gain transformation
  class Gain : public Complex2 {

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

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector< Jones<double> >*);
   
  };

}

#endif
