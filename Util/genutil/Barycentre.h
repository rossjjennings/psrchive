//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/Barycentre.h

#ifndef __Baricentre_H
#define __Baricentre_H

#include "MJD.h"
#include "sky_coord.h"
#include "Vector.h"

class Barycentre
{
 public:

  Barycentre ();

  void set_epoch (const MJD&);
  void set_coordinates (const sky_coord&);
  void set_observatory_xyz (double x, double y, double z);

  /*! returns D = 1-v/c, where v is the velocity of the observatory with respect 
      to the Solar System barycentre, projected onto the line of sight toward
      the pulsar, and c is the speed of light; v is +ve when the observatory
      is moving toward the pulsar.
      
      Therefore, topocentric frequency f_topo = f_SSB / D
  */
  double get_Doppler () const;
  MJD    get_barycentric_epoch () const;

  /*! returns the velocity of the observatory with respect to the geocentre */
  Vector<3,double> get_geocentric_velocity(const MJD&) const;

  bool verbose;

 private:

  MJD epoch;
  sky_coord coordinates;
  Vector<3, double> observatory;

  void build () const;
  mutable bool built;
  mutable double Doppler;
  mutable MJD barycentric_epoch;
};

#endif
