//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Univariate.h,v $
   $Revision: 1.4 $
   $Date: 2005/03/20 08:45:44 $
   $Author: straten $ */

#ifndef __MEAL_Univariate_H
#define __MEAL_Univariate_H

#include "MEAL/UnivariateBase.h"

namespace MEAL {

  //! A function of one scalar variable
  class Univariate : public UnivariateBase {

  public:

    //! Default constructor
    Univariate ();

    //! Destructor
    ~Univariate ();

    //! Copy constructor
    Univariate (const Univariate&);

    //! Assignment operator
    const Univariate& operator = (const Univariate&);

    //! Set the abscissa value
    void set_abscissa (double value);

    //! Get the abscissa value
    double get_abscissa () const;

  protected:

    //! The abscissa value
    double abscissa;

  };

}

#endif
