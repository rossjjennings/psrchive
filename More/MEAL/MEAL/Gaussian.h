//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gaussian.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/06 20:23:36 $
   $Author: straten $ */

#ifndef __Gaussian_H
#define __Gaussian_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Gaussian function 
  class Gaussian : public Univariate<Scalar> {

  public:

    Gaussian ();

    //! Set the centre
    void set_centre (double centre);

    //! Get the centre
    double get_centre () const;

    //! Set the width
    void set_width (double width);

    //! Get the width
    double get_width () const;

    //! Set the height
    void set_height (double height);

    //! Get the height
    double get_height () const;

    //! Set the cyclic flag
    void set_cyclic (bool cyclic);

    //! Get the cyclic flag
    bool get_cyclic () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, std::vector<double>* grad=0);

    //! Set true when the Gaussian is periodic
    bool cyclic;

  private:

    //! Parameter policy
    Parameters parameters;

  };

}

#endif
