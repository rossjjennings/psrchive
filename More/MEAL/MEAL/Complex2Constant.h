//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Constant.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Complex2Constant_H
#define __Complex2Constant_H

#include "Calibration/ConstantModel.h"
#include "Calibration/Complex2.h"

namespace Calibration {

  //! Represents a constant complex 2x2 matrix value
  class Complex2Constant : public ConstantModel, public Complex2 {

  public:

    //! Default constructor
    Complex2Constant (const Jones<double>& jones);

    //! Copy constructor
    Complex2Constant (const Complex2Constant& scalar);

    //! Assignment operator
    const Complex2Constant& operator = (const Complex2Constant& scalar);

    //! Destructor
    ~Complex2Constant ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Complex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the Jones matrix 
    Jones<double> evaluate (vector<Jones<double> >* gradient=0) const;
    
  protected:

    //! The Jones matrix returned by evaluate
    Jones<double> jones;

  };

}

#endif
