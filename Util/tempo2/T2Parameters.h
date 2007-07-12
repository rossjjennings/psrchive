//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Parameters.h,v $
   $Revision: 1.3 $
   $Date: 2007/07/12 05:59:40 $
   $Author: straten $ */

#ifndef __Tempo2Parameters_h
#define __Tempo2Parameters_h

#include "Pulsar/Parameters.h"

#include "tempo2.h"

namespace Tempo2 {

  class Generator;

  //! Storage of pulsar parameters used to create a Predictor
  /*! This pure virtual base class defines the interface to pulsar
    parameters */
  class Parameters : public Pulsar::Parameters {

  public:

    //! Default constructor
    Parameters ();

    //! Destructor
    ~Parameters ();

    //! Copy constructor
    Parameters (const Parameters&);

    //! Return a new, copy constructed instance of self
    Pulsar::Parameters* clone () const;

    //! Return a new Generator set up to produce a new Predictor
    Pulsar::Generator* generator () const;

    //! Return true if *this == *that
    bool equals (const Pulsar::Parameters* that) const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

    //! Return the name of the source
    std::string get_name () const;
    
    //! Return the coordinates of the source
    sky_coord get_coordinates () const;
    
    //! Return the dispersion measure
    double get_dispersion_measure () const;
    
    //! Return the rotation measure
    double get_rotation_measure () const;
    
  protected:

    friend class Generator;

    // the tempo2 pulsar structure defined in tempo2.h
    pulsar psr;

  };

}

#endif
