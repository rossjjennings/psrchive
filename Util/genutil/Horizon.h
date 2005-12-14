//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Horizon.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/14 05:43:40 $
   $Author: straten $ */

#ifndef __Horizon_H
#define __Horizon_H

#include "MJD.h"

class sky_coord;

namespace Calibration {

  //! Calculates horizon pointing parameters using SLALIB

  class Horizon  {

  public:

    //! Default constructor
    Horizon ();

    //! Set the coordinates of the source
    void set_source_coordinates (const sky_coord& coords);

    //! Set the latitude and longitude of the observatory in degrees
    void set_observatory_coordinates (double latitude, double longitude);

    //! Set the epoch in Modified Julian Days
    void set_epoch (const MJD& epoch);

    //! Get the epoch in Modified Julian Days
    MJD get_epoch () const;

    //! Get the hour_angle in radians
    double get_hour_angle () const;

    //! Get the parallactic angle in radians
    double get_parallactic_angle () const;

    //! Get the azimuth angle in radians
    double get_azimuth () const;

    //! Get the elevation angle in radians
    double get_elevation () const;

    //! Get the zenith angle in radians
    double get_zenith () const;

  protected:

    //! The declination of the source in radians
    double declination;

    //! The right ascension of the source in radians
    double right_ascension;

    //! The latitude of the observatory in degrees
    double latitude;

    //! The latitude of the observatory in degrees
    double longitude;

    //! The epoch
    MJD epoch;

    //! Flag set when return values have been computed
    bool built;

    //! Recomputes, when necessary, the following values
    void build () const {
      if (!built)
	const_cast<Horizon*>(this)->do_build();
    }

    //! Recomputes the following values
    void do_build ();

    //! The hour angle in radians
    double hour_angle;

    //! The parallactic angle in radians
    double parallactic_angle;

    //! The telescope azimuth in radians
    double azimuth;

    //! The telescope elevation in radians
    double elevation;

  };

}

#endif
