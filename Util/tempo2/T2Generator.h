//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Generator.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/08 06:57:12 $
   $Author: straten $ */

#ifndef __Tempo2Generator_h
#define __Tempo2Generator_h

#include "Generator.h"

namespace Tempo2 {

  class Predictor;

  //! Functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictors */
  class Generator : public Pulsar::Generator {

  public:

    //! Default constructor
    Generator ();

    //! Copy constructor
    Generator (const Generator&);

    //! Destructor
    ~Generator ();

    //! Return a new Predictor instance
    Pulsar::Predictor* generate () const;

  private:

  };

}

#endif
