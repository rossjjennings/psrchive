//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Polynomial2D.h

#ifndef __MEAL_Polynomial2D_H
#define __MEAL_Polynomial2D_H

#include "MEAL/Multivariate.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Polynomial.h"

namespace MEAL {

  //! Two-dimensional polynomial function 
  class Polynomial2D : public Multivariate<Scalar>
  {
    std::pair<unsigned, unsigned> ncoef;

  public:

    Polynomial2D (unsigned ncoeff = 0, unsigned mcoeff = 0);

    Polynomial2D (const Polynomial2D& copy);

    const Polynomial2D& operator = (const Polynomial2D& copy);

    //! Clone operator
    Polynomial2D* clone () const { return new Polynomial2D(*this); }

    std::string get_name() const;

    //! Textual interface to Function attributes
    class Interface;

    //! Return a text interface that can be used to access this instance
    TextInterface::Parser* get_interface ();

    const std::pair<unsigned,unsigned>& get_ncoef () const 
    { return ncoef; }

    void set_ncoef (const std::pair<unsigned,unsigned>& nc)
    { resize (nc.first, nc.second); }

    void resize (unsigned ncoeff, unsigned mcoeff);

    //! Set the abscissa value
    void set_abscissa_value (unsigned dim, double value);

    //! Set the abscissa offset
    void set_abscissa_offset (unsigned dim, double x0);

  protected:
    MEAL::ChainRule<MEAL::Scalar> chain;

    //! Polynomial along first abscissa
    MEAL::Polynomial poly;

    //! Polynomial variation of polynomial coefficients along second abscissa
    std::vector<MEAL::Polynomial> coefficients;

    void init ();

    void calculate (double& result, std::vector<double>* grad)
    { result = evaluation_policy->evaluate (grad); }

  };

}

#endif
