//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Horizon.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/14 16:16:18 $
   $Author: straten $ */

#ifndef __Horizon_H
#define __Horizon_H

#include "MJD.h"
#include "sky_coord.h"

//! Calculates horizon pointing parameters using SLALIB

class Horizon  {
  
public:
  
  //! Default constructor
  Horizon ();
  
  //! Set the coordinates of the source
  void set_source_coordinates (const sky_coord& coords);
  
  //! Set the latitude of the observatory in radians
  void set_observatory_latitude (double latitude);
  double get_observatory_latitude () const;

  //! Set the longitude of the observator in radians
  void set_observatory_longitude (double longitude);
  double get_observatory_longitude () const;

  //! Set the epoch in Modified Julian Days
  void set_epoch (const MJD& epoch);
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

  //! The latitude of the observatory in radians
  double latitude;

  //! The latitude of the observatory in radians
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

#endif
