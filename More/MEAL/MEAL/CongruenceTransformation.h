//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CongruenceTransformation.h,v $
   $Revision: 1.5 $
   $Date: 2005/04/06 20:23:36 $
   $Author: straten $ */

#ifndef __MEAL_CongruenceTransformation_H
#define __MEAL_CongruenceTransformation_H

#include "MEAL/Complex2.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! A congruence transformation, \f$\rho^\prime=J\rho J^\dagger\f$.
  /*! This class models the transformation of the matrix, \f$ \rho \f$ by
    a congruence transformation in \f$ J \f$.  The partial derivatives of the
    output, \f$ \rho^\prime \f$, are computed using the product rule
    and the partial derivatives of \f$ \rho \f$ and \f$ J \f$. */
  class CongruenceTransformation : public Complex2
  {

  public:

    //! Default constructor
    CongruenceTransformation ();

    //! Destructor
    ~CongruenceTransformation ();

    //! Set the transformation, \f$ J \f$
    virtual void set_transformation (Complex2* xform);

    //! Get the transformation, \f$ J \f$
    virtual Complex2* get_transformation ();

    //! Set the input, \f$ \rho \f$
    virtual void set_input (Complex2* xform);

    //! Get the input, \f$ \rho \f$
    virtual Complex2* get_input ();

  protected:

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! The input, \f$ \rho \f$
    Project<Complex2> input;

    //! The transformation, \f$ J \f$
    Project<Complex2> transformation;

    //! Composite parameter policy
    Composite composite;

  };

}

#endif

