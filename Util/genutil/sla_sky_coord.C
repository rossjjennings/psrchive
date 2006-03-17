/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "sky_coord.h"

#include <slalib.h>

// redwards -- function to construct from Galactic coordinates
const sky_coord&
sky_coord::setGalactic(AnglePair &gal)
{
  double l, b;
  double ra, dec;

  l = gal.angle1.getradians();
  b = gal.angle2.getradians();

  slaGaleq (l, b, &ra, &dec);

  angle1.setradians(ra);
  angle2.setradians(dec);
  return *this;
}

AnglePair
sky_coord::getGalactic() const
{
  double l, b;
  double ra, dec;
  AnglePair gal;

  ra = angle1.getradians();
  dec= angle2.getradians();

  slaEqgal (ra, dec, &l, &b);

  gal.angle1.setradians(l);
  gal.angle2.setradians(b);
  return gal;
}

