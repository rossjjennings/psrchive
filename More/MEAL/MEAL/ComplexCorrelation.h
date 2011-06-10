//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ComplexCorrelation.h,v $
   $Revision: 1.1 $
   $Date: 2010/05/24 06:33:56 $
   $Author: straten $ */

#ifndef __MEAL_ComplexCorrelation_H
#define __MEAL_ComplexCorrelation_H

#include "MEAL/Complex.h"

namespace MEAL
{

  //! A correlation coefficient on the unit disk in the complex plane
  class ComplexCorrelation : public Complex
  {

  public:

    //! Default constructor
    ComplexCorrelation ();

    //! Set the real part
    void set_real (const Estimate<double>& x)
    { this->set_Estimate (0, x); }

    //! Get the real part
    Estimate<double> get_real () const
    { return this->get_Estimate (0); }

    //! Set the imaginary part
    void set_imag (const Estimate<double>& y)
    { this->set_Estimate (1, y); }

    //! Get the imaginary part
    Estimate<double> get_imag () const
    { return this->get_Estimate (1); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "ComplexCorrelation"; }

  protected:

    typedef std::complex<double> Return;

    //! Calculate the result and its gradient
    void calculate (Return& result, std::vector<Return>* gradient);
  };

}

#endif
