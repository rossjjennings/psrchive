//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CongruenceTransformation.h,v $
   $Revision: 1.4 $
   $Date: 2004/11/23 12:17:50 $
   $Author: straten $ */

#ifndef __MEAL_CongruenceTransformation_H
#define __MEAL_CongruenceTransformation_H

#include "MEAL/Optimized.h"
#include "MEAL/Composite.h"
#include "MEAL/Complex2.h"

namespace MEAL {

  //! A congruence transformation, \f$\rho^\prime=J\rho J^\dagger\f$.
  /*! This class models the transformation of the matrix, \f$ \rho \f$ by
    a congruence transformation in \f$ J \f$.  The partial derivatives of the
    output, \f$ \rho^\prime \f$, are computed using the product rule
    and the partial derivatives of \f$ \rho \f$ and \f$ J \f$. */
  class CongruenceTransformation
    : public Optimized<Complex2>, public Composite 
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

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >* grad = 0);

  protected:

    //! The input, \f$ \rho \f$
    Project<Complex2> input;

    //! The transformation, \f$ J \f$
    Project<Complex2> transformation;

  };

}

#endif

