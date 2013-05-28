//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ConwayKronberg1969.h

#ifndef __CalibrationConwayKronberg1969_H
#define __CalibrationConwayKronberg1969_H

#include "Pulsar/BackendFeed.h"

namespace Calibration {

  class Feed;

  //! Parameterizes the feed as in Appendix II of Conway & Kronberg (1969)
  /*! This model suffers from the weakness that the phase of each
    cross-coupling term becomes ill-defined as the magnitude of the
    cross-coupling approaches zero.
  */
  class ConwayKronberg1969 : public BackendFeed {

  public:

    //! Default Constructor
    ConwayKronberg1969 ();

    //! Copy Constructor
    ConwayKronberg1969 (const ConwayKronberg1969& s);

    //! Assignment Operator
    const ConwayKronberg1969& operator = (const ConwayKronberg1969& s);

    //! Clone operator
    ConwayKronberg1969* clone () const;

    //! Destructor
    ~ConwayKronberg1969 ();

    //! Model the ellipticities of both receptors using the same value
    void equal_ellipticities ();

    //! Model the orientations of both receptors using the same value
    void equal_orientations ();

    //! Set cyclical limits on the model parameters
    void set_cyclic (bool);

    //! Fix the orientation of the frontend
    void set_constant_orientation (bool);
    
    const MEAL::Complex2* get_frontend () const;

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

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

