#include <stdio.h>
#include <math.h>

#include "sky_coord.h"
#include "f772c.h"

extern "C" double F772C(sla_galeq)(double *, double *, double *, double *);
extern "C" double F772C(sla_eqgal)(double *, double *, double *, double *);

sky_coord & sky_coord::operator= (const sky_coord & co)
{
  if (this != &co) {
    angle1 = co.angle1;
    angle2 = co.angle2;
  }
  return *this;
}

// ra and dec given in milliseconds of an hour
// JRa and JDec are stored in radians
const sky_coord& sky_coord::setJRaDecMS(long int ra, long int dec)
{
  angle1 = double(ra)  * M_PI / MilliSecin12Hours;
  angle2 = double(dec) * M_PI / MilliSecin12Hours;
  return *this;
}

// redwards -- function to construct from Galactic coordinates
const sky_coord&
sky_coord::setGalactic(AnglePair &gal)
{
  double l, b;
  double ra, dec;

  l = gal.angle1.getradians();
  b = gal.angle2.getradians();

  F772C(sla_galeq)(&l, &b, &ra, &dec);

  angle1.setradians(ra);
  angle2.setradians(dec);
  return *this;
}

AnglePair
sky_coord::getGalactic()
{
  double l, b;
  double ra, dec;
  AnglePair gal;

  ra = angle1.getradians();
  dec= angle2.getradians();

  F772C(sla_eqgal)(&ra, &dec, &l, &b);

  gal.angle1.setradians(l);
  gal.angle2.setradians(b);
  return gal;
}


#if 0
void sky_coord::print (FILE* out) const
{
  double hours   = JRa * 12.0 / M_PI;
  double degrees = JDec* 180.0 / M_PI;

  int rah, ram, ras;
  int ded, dem, des;

  rah = int (hours);
  hours -= double (rah);
  if (hours < 0.0)
    hours = - hours;
  hours *= 60.0;
  ram = int (hours);
  hours -= double (ram);
  hours *= 60.0;
  ras = int (hours);
  hours -= double (ras);
  printf ("JRA:  %02d:%02d:%02d%4.3lf   ", rah, ram, ras, hours);

  ded = int (degrees);
  degrees -= double (ded);
  if (degrees < 0.0)
    degrees = - degrees;
  degrees *= 60.0;
  dem  = int (degrees);
  degrees -= double (dem);
  degrees *= 60.0;
  des  = int (degrees);
  printf ("JDEC:  %02d:%02d:%02d%4.3lf\n", ded, dem, des, degrees);
}
#endif


