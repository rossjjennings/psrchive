//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/MuellerTransformation.h,v $
   $Revision: 1.1 $
   $Date: 2006/09/12 08:03:31 $
   $Author: straten $ */

#ifndef __MEAL_MuellerTransformation_H
#define __MEAL_MuellerTransformation_H

#include "MEAL/Complex2.h"
#include "MEAL/Real4.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! A Mueller transformation
  /*! This class models the transformation of the matrix, \f$ \rho \f$ by
    a Mueller matrix \f$ M \f$.  The partial derivatives of the
    output, \f$ \rho^\prime \f$, are computed using the product rule
    and the partial derivatives of \f$ \rho \f$ and \f$ M \f$. */
  class MuellerTransformation : public Complex2
  {

  public:

    //! Default constructor
    MuellerTransformation ();

    //! Destructor
    ~MuellerTransformation ();

    //! Set the transformation, \f$ M \f$
    virtual void set_transformation (Real4* xform);

    //! Get the transformation, \f$ J \f$
    virtual Real4* get_transformation ();

    //! Set the input, \f$ \rho \f$
    virtual void set_input (Complex2* xform);

    //! Get the input, \f$ \rho \f$
    virtual Complex2* get_input ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! The input, \f$ \rho \f$
    Project<Complex2> input;

    //! The transformation, \f$ M \f$
    Project<Real4> transformation;

    //! Composite parameter policy
    Composite composite;

  };

}

#endif

