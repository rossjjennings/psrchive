//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Boost.h,v $
   $Revision: 1.5 $
   $Date: 2005/08/18 12:10:42 $
   $Author: straten $ */

#ifndef __MEAL_Boost_H
#define __MEAL_Boost_H

#include "MEAL/Complex2.h"
#include "MEAL/Parameters.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a boost (Hermitian, dichroic) transformation
  /*! This class represents the boost, \f$\beta\f$, along an arbitrary
    axix, \f$\hat m\f$. */
  class Boost : public Complex2 {

  public:

    Boost ();

    //! Construct with a fixed axis
    Boost (const Vector<3, double>& axis);

    //! Fix the axis along which the boost occurs
    void set_axis (const Vector<3, double>& axis);

    //! Get the unit-vector along which the boost occurs
    Vector<3, double> get_axis (double* beta = 0) const;

    //! Set the boost parameter, beta
    void set_beta (double beta);

    //! Get the boost parameter, beta
    double get_beta () const;

    //! Free the axis along which the boost occurs
    void free_axis ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! The axis along which the boost occurs
    Vector<3, double> axis;

    //! Calculate the Jones matrix and its gradient when axis is free
    void calculate_Gibbs (Jones<double>&, std::vector<Jones<double> >*);

    //! Calculate the Jones matrix and its gradient when axis is fixed
    void calculate_beta (Jones<double>&, std::vector<Jones<double> >*);

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

  private:

    //! Parameter policy
    Parameters parameters;

  };

}

#endif
