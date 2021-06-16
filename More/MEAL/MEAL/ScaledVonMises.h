//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScaledVonMises.h

#ifndef __ScaledVonMises_H
#define __ScaledVonMises_H

#include "MEAL/ProductRule.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"
#include "MEAL/ScalarMath.h"

namespace MEAL {

  //! ScaledVonMises function 
  class ScaledVonMises :
      public Univariate<Scalar> {

  public:

    ScaledVonMises (bool log_height = false);
    ScaledVonMises (const ScaledVonMises& copy);
    ScaledVonMises & operator = (const ScaledVonMises& copy);

    //! Clone operator
    ScaledVonMises* clone () const { return new ScaledVonMises(*this); }

    //! Set the centre
    void set_centre (const Estimate<double> &centre);

    //! Get the centre
    Estimate<double> get_centre () const;

    //! Set the concentration
    void set_concentration (const Estimate<double> &concentration);

    //! Get the concentration
    Estimate<double> get_concentration () const;

    //! Set the width
    void set_width (double width);

    //! Get the width
    double get_width () const;

    //! Set the height
    void set_height (const Estimate<double> &height);

    //! Get the height 
    Estimate<double> get_height () const; 
 
    //! Connect the set_abscissa method to the axis value
    //   void set_argument (unsigned dimension, Argument* axis);

    //! Get the area under the curve
    double get_area () const;
    
    std::string get_name() const;

    //! Parses the values of model parameters and fit flags from a string
    //    void parse (const std::string& text);
 
  protected:
    ScalarParameter height;
    ScalarParameter centre;
    ScalarParameter concentration;

    //! When set, the height attribute is interpreted as log(height) [ensures that height > 0]
    bool log_height;

    void init ();
    Reference::To<Scalar> expression;
    void calculate (double&, std::vector<double>*) {}

  };

}

#endif
