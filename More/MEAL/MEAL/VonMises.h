//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/VonMises.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __VonMises_H
#define __VonMises_H

#include "MEAL/ProductRule.h"
//#include "MEAL/ArgumentBehaviour.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"
#include "MEAL/ScalarMath.h"

namespace MEAL {

  //! VonMises function 
  class VonMises : //public ArgumentBehaviour, 
      public ProductRule<Scalar> {

  public:

    VonMises ();

    //! Set the centre
    void set_centre (double centre);

    //! Get the centre
    double get_centre () const;

    //! Set the concentration
    void set_concentration (double concentration);

    //! Get the concentration
    double get_concentration () const;

    //! Set the height
    void set_height (double height);

    //! Get the height
    double get_height () const;

    //! Connect the set_abscissa method to the axis value
    void set_argument (unsigned dimension, Argument* axis);

    std::string get_name() const;

    //! Parses the values of model parameters and fit flags from a string
    void parse (const std::string& text);
  
  protected:
    ScalarArgument x;
    ScalarParameter centre;
    ScalarParameter concentration;

  };

}

#endif
