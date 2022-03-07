//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/MultivariatePolicy.h

#ifndef __MEAL_MultivariatePolicy_H
#define __MEAL_MultivariatePolicy_H

#include "MEAL/ArgumentPolicy.h"

namespace MEAL {

  //! A function of one or more scalar variables
  class MultivariatePolicy : public ArgumentPolicy
  {
  public:

    //! Default constructor
    MultivariatePolicy (unsigned ndim, Function* context);

    //! Copy constructor
    MultivariatePolicy (const MultivariatePolicy&);

    //! Desctructor
    ~MultivariatePolicy ();

    //! Assignment operator
    const MultivariatePolicy& operator = (const MultivariatePolicy&);

    //! Clone operator
    MultivariatePolicy* clone (Function* context) const;

    //! Set the abscissa value
    void set_abscissa (unsigned idim, double value);

    //! Get the abscissa value
    double get_abscissa (unsigned idim) const;

    //! If Argument is an Axis<double>, connect it to set_abscissa
    void set_argument (unsigned idim, Argument* axis);

  protected:

    //! The abscissa values
    std::vector<double> abscissa;

  };

}

#endif

