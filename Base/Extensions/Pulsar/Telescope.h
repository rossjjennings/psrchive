//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Telescope.h,v $
   $Revision: 1.1 $
   $Date: 2004/06/18 11:32:53 $
   $Author: straten $ */

#ifndef __Telescope_Extension_h
#define __Telescope_Extension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! Contains information about the telescope used for the observation
  class Telescope : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    Telescope ();

    //! Copy constructor
    Telescope (const Telescope& extension);

    //! Operator =
    const Telescope& operator= (const Telescope& extension);

    //! Destructor
    ~Telescope ();

    //! Clone method
    Telescope* clone () const { return new Telescope(*this); }

    //////////////////////////////////////////////////////////////////////
    //
    // Information about the primary reflector
    //

    //! Types of primary reflector mounts
    enum Mount {
      //! Equatorial (right ascension, declination)
      Equatorial,
      //! Horizon (altidue, azimuth)
      Horizon,
      //! Fixed (LOFAR, Arecibo)
      Fixed,
      //! Mobile (satellite)
      Mobile
    };

    //! Types of primary reflector
    enum Reflector {
      //! Parabolic
      Parabolic,
      //! Spherical
      Spherical
    };

    //! Types of focus
    enum Focus {
      /*! Prime Focus: primary reflector; receiver at primary focus */
      PrimeFocus,
      /*! Cassegrain: primary and secondary reflectors; receiver at
	secondary focus; prime focus is behind secondary reflector */
      Cassegrain,
      /*! Gregorian: primary and secondary reflectors; receiver at
	secondary focus; prime focus is in front of secondary reflector */
      Gregorian
    };

    //! Name of the observatory and/or primary reflector
    string name;

    //! Mount type of primary reflector
    Mount mount;

    //! Type of primary reflector
    Reflector primary;

    //! Type of focus
    Focus focus;

  };

}

#endif
