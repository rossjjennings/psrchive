//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CongruenceTransformation.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Calibration_CongruenceTransformation_H
#define __Calibration_CongruenceTransformation_H

#include "Calibration/OptimizedModel.h"
#include "Calibration/Composite.h"
#include "Calibration/Complex2.h"

namespace Calibration {

  //! Models the congruence transformation, \f$\rho^prime=J\rho J^\dagger\f$.
  /*! This class models the transformation of the matrix, \f$ \rho \f$ by
    a congruence transformation in \f$ J \f$.  The partial derivatives of the
    output, \f$ \rho^\prime \f$, are computed using the product rule
    and the partial derivatives of \f$ \rho \f$ and \f$ J \f$. */
  class CongruenceTransformation
    : public OptimizedModel<Complex2>, public Composite 
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
    // OptimizedModel implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, vector<Jones<double> >* grad = 0);

  protected:

    //! The input, \f$ \rho \f$
    Project<Complex2> input;

    //! The transformation, \f$ J \f$
    Project<Complex2> transformation;

  };

}

#endif

