//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/Britton2000.h

#ifndef __CalibrationBritton2000_H
#define __CalibrationBritton2000_H

#include "Pulsar/BackendFeed.h"

namespace Calibration {

  class Feed;

  //! Phenomenological description of the instrument
  /*! The transformation is represented by the product of a SingleAxis
    transformation, a two-dimensional boost and a two-dimensional
    rotation.

    Note that the free parameters are equal to one half of the values
    shown in Equation 19 of Britton (2000).  For example,

    b_1 = \delta_theta / 2
  */
  class Britton2000 : public BackendFeed {

  public:

    //! Default Constructor
    Britton2000 (bool isolate_degeneracy = false);

    //! Copy Constructor
    Britton2000 (const Britton2000& s);

    //! Assignment Operator
    const Britton2000& operator = (const Britton2000& s);

    //! Clone operator
    Britton2000* clone () const;

    //! Destructor
    ~Britton2000 ();

    //! Model the ellipticities of both receptors using the same value
    void equal_ellipticities ();

    //! Model the orientations of both receptors using the same value
    void equal_orientations ();

    //! Fix the orientation of the frontend
    void set_constant_orientation (bool);
    bool get_constant_orientation () const;
    
    const MEAL::Complex2* get_frontend () const;

    bool get_degeneracy_isolated() const { return isolate_degeneracy; }
    
    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    class Feed;

    //! Feed model
    Reference::To<Feed> feed;
    bool isolate_degeneracy;
    
  private:

    //! Initialize function used by constructors
    void init (bool iso);

  };

}

#endif

