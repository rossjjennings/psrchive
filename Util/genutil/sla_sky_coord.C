#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sky_coord.h"

#define SLA_galeq SLA_FUNC(sla_galeq,slaGaleq)
extern "C" double SLA_galeq (double *, double *, double *, double *);

#define SLA_eqgal SLA_FUNC(sla_eqgal,slaEqgal)
extern "C" double SLA_eqgal(double *, double *, double *, double *);

// redwards -- function to construct from Galactic coordinates
const sky_coord&
sky_coord::setGalactic(AnglePair &gal)
{
  double l, b;
  double ra, dec;

  l = gal.angle1.getradians();
  b = gal.angle2.getradians();

  SLA_galeq (&l, &b, &ra, &dec);

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

  SLA_eqgal (&ra, &dec, &l, &b);

  gal.angle1.setradians(l);
  gal.angle2.setradians(b);
  return gal;
}

