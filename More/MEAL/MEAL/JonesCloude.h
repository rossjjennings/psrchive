//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/JonesCloude.h

#ifndef __MEAL_JonesCloude_H
#define __MEAL_JonesCloude_H

#include "MEAL/Complex4.h"
#include "MEAL/Complex2.h"
#include "MEAL/Convert.h"

namespace MEAL {

  //! Converts a Jones matrix into a Cloude target coherency matrix
  class JonesCloude : public Convert<Complex2,Complex4>
  {

  public:

    //! Default constructor
    JonesCloude (Complex2* = 0);

    //! Set the transformation, \f$ J \f$
    virtual void set_transformation (Complex2* model) { set_model(model); }

    //! Get the transformation, \f$ J \f$
    virtual Complex2* get_transformation () { return get_model(); }

    std::string get_name () const;

  protected:

    //! Calculate the Cloude matrix and its gradient
    virtual void calculate (Result& result, std::vector<Result>*);
  };

}

#endif

