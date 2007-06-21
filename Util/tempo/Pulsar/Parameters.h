//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/Parameters.h,v $
   $Revision: 1.3 $
   $Date: 2007/06/21 17:32:41 $
   $Author: straten $ */

#ifndef __PulsarParameters_h
#define __PulsarParameters_h

#include "Reference.h"
#include "sky_coord.h"

#include <stdio.h>

namespace Pulsar {

  class Generator;

  //! Storage of pulsar parameters used to create a Predictor
  /*! This pure virtual base class defines the interface to pulsar
    parameters */
  class Parameters : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Return a new, copy constructed instance of self
    virtual Parameters* clone () const = 0;

    //! Return a new Generator set up to produce a new Predictor
    virtual Generator* generator () const = 0;

    //! Return true if *this == *that
    virtual bool equals (const Parameters* that) = 0;

    //! Load from an open stream
    virtual void load (FILE*) = 0;

    //! Unload to an open stream
    virtual void unload (FILE*) const = 0;

    // ***********************************************************************
    //
    // Please add attributes sparingly.  Every pure virtual method added will
    // impact on at least two other derived classes.
    //
    // ***********************************************************************

    //! Return the name of the source
    virtual std::string get_name () const = 0;

    //! Return the coordinates of the source
    virtual sky_coord get_coordinates () const = 0;

    //! Return the dispersion measure
    virtual double get_dispersion_measure () const = 0;

    //! Return the rotation measure
    virtual double get_rotation_measure () const = 0;

    //! Factory helper creates a vector of pointers to derived class instances
    static void children (std::vector< Reference::To<Parameters> >&);

  };

}

#endif
