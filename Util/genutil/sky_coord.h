#ifndef __SKY_COORD_H
#define __SKY_COORD_H


#include "angle.h"

// Class for sky coordinates. Inherits AnglePair, where members
// angle1 and angle2 are J2000.0 right ascension and declination
// respectively.
class sky_coord  : public AnglePair
{
  //  friend class sky_coord_gui;

  // protected:
  //  double JRa;
  //  double JDec;

 public:
   sky_coord () : AnglePair() {};
   sky_coord (const sky_coord & co) { *this = co; };
   sky_coord & operator= (const sky_coord & co);
   sky_coord(const Angle & a1, const Angle & a2) : AnglePair(a1,a2) {};
   sky_coord(const double d1, const double d2) : AnglePair(d1,d2) {};

   const sky_coord& setJRaDecMS(long int ra, long int dec);
   const sky_coord& setGalactic(AnglePair &gal);
   AnglePair getGalactic();
};

#endif //SKY_COORD_H
