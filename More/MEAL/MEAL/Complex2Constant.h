//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Constant.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

#ifndef __Complex2Constant_H
#define __Complex2Constant_H

#include "MEAL/Constant.h"
#include "MEAL/Complex2.h"

namespace MEAL {

  //! Represents a constant complex 2x2 matrix value
  class Complex2Constant : public Constant, public Complex2 {

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
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Complex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the Jones matrix 
    Jones<double> evaluate (std::vector<Jones<double> >* gradient=0) const;
    
  protected:

    //! The Jones matrix returned by evaluate
    Jones<double> jones;

  };

}

#endif
