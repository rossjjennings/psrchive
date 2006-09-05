//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Parallactic.h,v $
   $Revision: 1.4 $
   $Date: 2006/09/05 22:53:23 $
   $Author: straten $ */

#ifndef __Calibration_Parallactic_H
#define __Calibration_Parallactic_H

#include "MEAL/Rotation1.h"
#include "MJD.h"

class sky_coord;

namespace Calibration {

  //! Parallactic angle transformation of receiver feed
  /*! The parallactic angle is also known as the Vertical Angle (VA).
    According to http://www.ucolick.org/~sla/deimos/swpdr/va.html

    "Note that in many applications this angle is mis-named the
    'Parallactic Angle'.  Lately this has been promoted by
    terminology found in the extremely useful SLALIB software by Pat
    Wallace, but the misnomer can be traced back at least as far as
    Spherical Astronomy by Smart.  The correct usage of Parallactic
    Angle is in the context of the Equatorial and Ecliptic coordinate
    systems.  In that context it describes the orientation of the
    ellipse that stellar coordinates traverse due to annual
    parallax.  The term 'Parallactic Angle' should not be applied in
    the context of the Horizon system."

    Nevertheless, the term 'Parallactic' is used to name this class.
    The following description also comes from the same web-site:

    "The declination of an object is delta, with north being positive.
    The latitude of an observer is phi, with north being positive.
    The Hour Angle of an object is HA; zero on the meridian, negative
    to the east, and positive to the west.  In the equatorial (RA,Dec)
    coordinate system the position angle of a directed vector on the
    sky is measured from equatorial North toward equatorial East; this
    is EquPA.  In the Horizon (Alt-Az) coordinate system the position
    angle of a directed vector on the sky is measured from Up toward
    an upright observer's Left; this is HorPA. [...]  The Vertical
    Angle (VA) is defined as the PA of the local vertical as expressed
    in the equatorial system: VA = EquPA - HorPA"

    This class represents the transformation from EquPA to HorPA as a
    rotation about the Stokes V axis by -VA.
  */

  class Parallactic : public MEAL::Rotation1 {

  public:

    //! Default constructor
    Parallactic ();

    //! Set the coordinates of the source
    void set_source_coordinates (const sky_coord& coords);

    //! Set the latitude and longitude of the observatory in degrees
    void set_observatory_coordinates (double latitude, double longitude);

    //! Set the MJD
    void set_epoch (const MJD& epoch);

    //! Get the MJD
    MJD get_epoch () const;

    //! Set the hour angle in radians
    void set_hour_angle (double hour_angle);

    //! Get the hour_angle in radians
    double get_hour_angle () const;

    //! Set the parallactic angle in radians
    void set_parallactic_angle (double pa);

    //! Get the parallactic angle in radians
    double get_parallactic_angle () const;

  protected:

    //! The declination of the source in radians
    double declination;

    //! The right ascension of the source in radians
    double right_ascension;

    //! The latitude of the observatory in degrees
    double latitude;

    //! The latitude of the observatory in degrees
    double longitude;

  private:

    //! The last epoch for which the result was calculated
    MJD current_epoch;

    //! The last hour angle for which the result was calculated
    double current_hour_angle;

    //! Resets the result whenever set_*_coordinates is called
    void reset ();

  };

}

#endif
