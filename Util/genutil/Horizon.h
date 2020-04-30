//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/Horizon.h

#ifndef __Horizon_H
#define __Horizon_H

#include "Directional.h"

//! Horizon mounted antenna with first rotation in horizontal plane
/*! Also known as altazimuth, az-el, Dobsonian, or even x-y mount */
class Horizon : public Directional
{  
public:

  //! Default constructor
  Horizon (double az=0, double el=0);
  
  //! Get the azimuth angle in radians
  double get_azimuth () const;

  //! Get the elevation angle in radians
  double get_elevation () const;

  //! Get the zenith angle in radians
  double get_zenith () const;

  //! Get the name of the mount
  std::string get_name () const;

  //! Return all possible slew times and resulting telescope states
  std::vector< std::pair<double,Mount*> >
  slew_times (const sky_coord& coords);

  std::vector< std::pair<double,Mount*> >
  slew_times (double az, double el);

  void set_minimum_azimuth (double a) { minimum_azimuth = a; }
  double get_minimum_azimuth () const { return minimum_azimuth; }

  void set_maximum_azimuth (double a) { maximum_azimuth = a; }
  double get_maximum_azimuth () const { return maximum_azimuth; }
  
  void set_minimum_elevation (double a) { minimum_elevation = a; }
  double get_minimum_elevation () const { return minimum_elevation; }

  void set_maximum_elevation (double a) { maximum_elevation = a; }
  double get_maximum_elevation () const { return maximum_elevation; }
  
  void set_azimuth_velocity (double a) { azimuth_velocity = a; }
  double get_azimuth_velocity () const { return azimuth_velocity; }

  void set_elevation_velocity (double a) { elevation_velocity = a; }
  double get_elevation_velocity () const { return elevation_velocity; }

protected:

  //! Get the receptor basis in the reference frame of the observatory
  Matrix<3,3,double> get_basis (const Vector<3,double>& from_source) const;

  //! The telescope azimuth in radians
  mutable double azimuth;

  //! The telescope elevation in radians
  mutable double elevation;

  double minimum_azimuth;
  double maximum_azimuth;
  
  double minimum_elevation;
  double maximum_elevation;
  
  double azimuth_velocity;
  double elevation_velocity;
};

#endif
