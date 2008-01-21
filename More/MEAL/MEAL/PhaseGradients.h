//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/PhaseGradients.h,v $
   $Revision: 1.1 $
   $Date: 2008/01/21 20:19:49 $
   $Author: straten $ */

#ifndef __MEAL_PhaseGradients_H
#define __MEAL_PhaseGradients_H

#include "MEAL/Univariate.h"
#include "MEAL/Complex2.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Multiple phase gradients parameterized by their slopes
  class PhaseGradients : public Univariate<Complex2> {

  public:

    //! Default constructor
    PhaseGradients (unsigned nslopes = 0);

    //! Copy constructor
    PhaseGradients (const PhaseGradients&);

    //! Assignment operator
    PhaseGradients& operator = (const PhaseGradients&);

    //! Clone operator
    PhaseGradients* clone () const;

    //! Set the current phase gradient
    void set_islope (unsigned islope);

    //! Add another slope to the set
    void add_slope ();

    //! Get the number of slopes
    unsigned get_nslope () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);
   
    //! Parameter policy
    Parameters parameters;

    //! The current phase gradient
    unsigned islope;

    //! Initialization
    void init ();
  };

}

#endif
