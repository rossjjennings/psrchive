//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Boost.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Calibration_Boost_H
#define __Calibration_Boost_H

#include "Calibration/OptimizedComplex2.h"
#include "Vector.h"

namespace Calibration {

  //! Represents a boost (Hermitian, dichroic) transformation
  /*! This class represents the boost, \f$\beta\f$, along an arbitrary
    axix, \f$\hat m\f$. */
  class Boost : public OptimizedComplex2 {

  public:

    Boost ();

    //! Construct with a fixed axis
    Boost (const Vector<double, 3>& axis);

    //! Fix the axis along which the boost occurs
    void set_axis (const Vector<double, 3>& axis);

    //! Get the unit-vector along which the boost occurs
    Vector<double, 3> get_axis (double* beta = 0) const;

    //! Set the boost parameter, beta
    void set_beta (double beta);

    //! Get the boost parameter, beta
    double get_beta () const;

    //! Free the axis along which the boost occurs
    void free_axis ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    //! Return the name of the specified parameter
    string get_param_name (unsigned index) const;

  protected:
    //! The axis along which the boost occurs
    Vector<double, 3> axis;

    //! Calculate the Jones matrix and its gradient when axis is free
    void calculate_Gibbs (Jones<double>& result, vector<Jones<double> >* grad);

    //! Calculate the Jones matrix and its gradient when axis is fixed
    void calculate_beta (Jones<double>& result, vector<Jones<double> >* grad);

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedComplex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, vector<Jones<double> >* gradient);

  };

}

#endif
