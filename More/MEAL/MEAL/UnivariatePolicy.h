//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnivariatePolicy.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:30:56 $
   $Author: straten $ */

#ifndef __MEAL_UnivariatePolicy_H
#define __MEAL_UnivariatePolicy_H

#include "MEAL/ArgumentPolicy.h"

namespace MEAL {

  //! A function of one scalar variable
  class UnivariatePolicy : public ArgumentPolicy {

  public:

    //! Default constructor
    UnivariatePolicy (Function* context);

    //! Copy constructor
    UnivariatePolicy (const UnivariatePolicy&);

    //! Desctructor
    ~UnivariatePolicy ();

    //! Assignment operator
    const UnivariatePolicy& operator = (const UnivariatePolicy&);

    //! Clone operator
    UnivariatePolicy* clone (Function* context) const;

    //! Set the abscissa value
    void set_abscissa (double value);

    //! Get the abscissa value
    double get_abscissa () const;

    //! If Argument is an Axis<double>, connect it to set_abscissa
    void set_argument (unsigned dimension, Argument* axis);

  protected:

    //! The abscissa value
    double abscissa;

  };

}

#endif

