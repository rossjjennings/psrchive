/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/sky_coord.h,v $
   $Revision: 1.4 $
   $Date: 2001/06/02 07:23:37 $
   $Author: straten $ */

#ifndef __SKY_COORD_H
#define __SKY_COORD_H


#include "angle.h"

// Class for sky coordinates. Inherits AnglePair, where members
// angle1 and angle2 are J2000.0 right ascension and declination
// respectively.
class sky_coord  : public AnglePair
{
 public:
   sky_coord () : AnglePair() {};

   // as long as sky_coord is empty, why not simply accept AnglePair?
   sky_coord (const AnglePair & co) { AnglePair::operator=(co); };
   sky_coord& operator= (const AnglePair & co)
     { AnglePair::operator=(co); return *this; }

   sky_coord (const Angle & a1, const Angle & a2) : AnglePair(a1,a2) {};
   sky_coord (const double d1, const double d2) : AnglePair(d1,d2) {};

   const sky_coord& setJRaDecMS(long int ra, long int dec);

   const sky_coord& setGalactic(AnglePair &gal);

   AnglePair getGalactic() const;

   AnglePair & getRaDec() { return *this; }

   Angle & ra()  { return angle1; }
   Angle & dec() { return angle2; }

   // const versions
   const AnglePair & getRaDec() const { return *this; }
   const Angle & ra() const  { return angle1; }
   const Angle & dec() const { return angle2; }
};

#endif //SKY_COORD_H
