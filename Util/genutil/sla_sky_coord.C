#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sky_coord.h"

#define F77_sla_galeq F77_SLA(sla_galeq,SLA_GALEQ)
extern "C" double F77_sla_galeq (double *, double *, double *, double *);

#define F77_sla_eqgal F77_SLA(sla_eqgal,SLA_EQGAL)
extern "C" double F77_sla_eqgal(double *, double *, double *, double *);

// redwards -- function to construct from Galactic coordinates
const sky_coord&
sky_coord::setGalactic(AnglePair &gal)
{
  double l, b;
  double ra, dec;

  l = gal.angle1.getradians();
  b = gal.angle2.getradians();

  F77_sla_galeq (&l, &b, &ra, &dec);

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

  F77_sla_eqgal (&ra, &dec, &l, &b);

  gal.angle1.setradians(l);
  gal.angle2.setradians(b);
  return gal;
}

