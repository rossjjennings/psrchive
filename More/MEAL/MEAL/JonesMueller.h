//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/JonesMueller.h,v $
   $Revision: 1.2 $
   $Date: 2008/06/15 17:09:05 $
   $Author: straten $ */

#ifndef __MEAL_JonesMueller_H
#define __MEAL_JonesMueller_H

#include "MEAL/Real4.h"
#include "MEAL/Complex2.h"
#include "MEAL/Convert.h"

namespace MEAL {

  //! Converts a Jones matrix into a Mueller matrix

  class JonesMueller : public Convert<Complex2,Real4>
  {

  public:

    //! Default constructor
    JonesMueller (Complex2* = 0);

    //! Set the transformation, \f$ J \f$
    virtual void set_transformation (Complex2* transformation)
    { set_model (transformation); }

    //! Get the transformation, \f$ J \f$
    virtual Complex2* get_transformation ()
    { return get_model (); }

    std::string get_name () const;

  protected:

    //! Calculate the Mueller matrix and its gradient
    virtual void calculate (Matrix<4,4,double>& result,
			    std::vector<Matrix<4,4,double> >*);

  };

}

#endif

