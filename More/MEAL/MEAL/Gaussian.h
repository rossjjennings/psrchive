//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gaussian.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:15 $
   $Author: straten $ */

#ifndef __Gaussian_H
#define __Gaussian_H

#include "Calibration/UnivariateOptimizedScalar.h"

namespace Calibration {

  //! Gaussian function 
  class Gaussian : public UnivariateOptimizedScalar {

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
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    //! Return the name of the specified parameter
    string get_param_name (unsigned index) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedModel implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, vector<double>* grad=0);

  protected:

    //! Set true when the Gaussian is periodic
    bool cyclic;

  };

}

#endif
