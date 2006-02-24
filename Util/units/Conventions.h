//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Conventions.h,v $
   $Revision: 1.1 $
   $Date: 2006/02/24 17:08:42 $
   $Author: straten $ */

#ifndef __Conventions_h
#define __Conventions_h

namespace Signal {

  //! The basis in which the electric field is represented
  enum Basis { Circular=0, Linear=1, Elliptical=2 };

  //! The hand of the basis
  enum Hand { Left=-1, Right=1 };

  //! The complex phase of the basis
  enum Argument { Conjugate=-1, Conventional=1 };

}

#endif

