//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Constant.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/06 20:23:36 $
   $Author: straten $ */

#ifndef __Complex2Constant_H
#define __Complex2Constant_H

#include "MEAL/Complex2.h"

namespace MEAL {

  //! Represents a constant complex 2x2 matrix value
  class Complex2Constant : public Complex2 {

  public:

    //! Default constructor
    Complex2Constant (const Jones<double>& jones);

    //! Assignment operator
    const Complex2Constant& operator = (const Complex2Constant& scalar);

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
    void calculate (Jones<double>&, std::vector<Jones<double> >*);
    
  protected:

    //! The Jones matrix returned by evaluate
    Jones<double> value;

  };

}

#endif
