
#include <stdio.h>
#include <string.h>
#include "angle.h"
#include "coord.h"

int main ()
{
  AnglePair coordinates;

  double l, b;
  float  lst, latitude;
  float  az, zen, para;
  char coordstr [80];

  strcpy (coordstr, "04:37:15.747849-47:15:08.23371");

  coordinates.setHMSDMS (coordstr);
  galactic (coordinates.angle1.getradians(), 
	    coordinates.angle2.getradians(), &l, &b);

  printf ("COORDSTR: %s\n", coordstr);
  printf ("(RA DEC) (%s)  l:%g   b:%g\n", coordinates.getHMSDMS().c_str(),
	  l*(180/M_PI), b*(180/M_PI));

  return 0;
}
