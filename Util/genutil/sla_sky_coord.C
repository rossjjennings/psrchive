#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include "f772c.h"
#ifndef F772C_H
#define F772C_H
#define F772C(x) x##_
#endif

#include "sky_coord.h"

extern "C" {
  double F772C(sla_galeq) (double *, double *, double *, double *);
}

extern "C"{
  double F772C(sla_galeq) (double *, double *, double *, double *);
}

// redwards -- function to construct from Galactic coordinates
const sky_coord&
sky_coord::setGalactic(AnglePair &gal)
{
  double l, b;
  double ra, dec;

  l = gal.angle1.getradians();
  b = gal.angle2.getradians();

  F772C(sla_galeq) (&l, &b, &ra, &dec);

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

  F772C(sla_galeq)(&ra, &dec, &l, &b);

  gal.angle1.setradians(l);
  gal.angle2.setradians(b);
  return gal;
}

