#include <stdio.h>
#include "coord.h"

/* **********************************************************************

   telescope_coords - given a TEMPO telescope code, returns the 
                      geodetic latitude and east longitude in degrees
		      as well as the elevation in metres.  If not 
		      interested in any of the values, simply pass NULL
		      in the call.

   ********************************************************************** */

int telescope_coords (char telescope, float* latitude, 
		      float* longitude, float* elevation)
{
  float latd=-999, longd=-999, elev=-1e9;
  
  if (telescope < 10) /* if the char is < 10 then it was probably an int */
    {
      fprintf(stderr,"telescope_coords(.c): Warning code is %c\n",
	      telescope);
     telescope = '0' + telescope;
    }
  switch (telescope) {

  case '1':
    /* GBT NRAO */
    latd = 38.433;
    longd = 79.84;
    elev = 807.43;
    break;

  case '2':
    /* ATCA Narrabri */
    latd = -30.31;
    longd = 149.57;
    elev = 217.0;
    break;

  case '3':
    /* Arecibo */
    latd = 18.345;
    longd = 293.247;
    elev = 496.0;
    break;

  case '4':
    /* Hobart */
    latd = 42.805;
    longd = 147.439;
    elev = 26.0;
    break;
    
  case '5':
    /* Urumqi */
    latd = 43.47;
    longd = 87.1778;
    elev = 2029.3;
    break;

  case '6':
    /* Tidbinbilla */
    latd = -35.4;
    longd = 148.98;
    elev = 670.0;
    break;

  case '0': 
    fprintf(stderr, "telescope_coords: 0 -- assuming Parkes!\n");
  case '7':
    /* Parkes */
    latd = -33.0;
    longd = 148.2617;
    elev = 392.0;
    break;

  case 'i':
    /* Westerbork, from:

       WSRT Site Information Form
       International GPS Service for Geodynamics
       http://igscb.jpl.nasa.gov/mail/igsmail/2000/msg00323.html

     */
    latd = 52.9146;
    longd = 6.6045;
    elev = 86.0000;
    break;

  default:
    fprintf (stderr, "telescope_coords: unknown telescope code %d\n",
	     telescope);
    return -1;
  }

  if (latitude)
    *latitude = latd;
  if (longitude)
    *longitude = longd;
  if (elevation)
    *elevation = elev;

  return 0;
}

/* **********************************************************************

   telescope_xyz - given a TEMPO telescope code, returns the coordinates 
                   of the site about the geocentre in metres.  If not 
		   interested in any of the values, simply pass NULL in 
                   the call.

		   NOTE: These values were gleaned from the TEMPO
                   obsys.dat file by AWH on 23/12/2003

   ********************************************************************** */

int telescope_xyz (char telescope, double* x_,
		      double* y_, double* z_)
{
  double x=0.0, y=0.0, z=0.0;
  
  if (telescope < 10) /* if the char is < 10 then it was probably an int */
     telescope = '0' + telescope;

  switch (telescope) {

  case '2':
    /* ATCA Narrabri */
    x = -4751359.712;
    y = 2792175.127;
    z = -3200491.700;
    break;

  case '3':
    /* Arecibo */
    x = 2390490.0;
    y = -5564764.0;
    z = 1994727.0;
    break;

  case '5':
    /* Nanshan, Urumqi */
    x = -228310.702;
    y = 4631922.905;
    z = 4367064.059;
    break;

  case '6':
    /* Tidbinbilla, DSS 43 */
    x = -4460892.6;
    y = 2682358.9;
    z = -3674756.0;
    break;

  case '0': 
    fprintf(stderr, "telescope_xyz: 0 -- assuming Parkes!\n");
  case '7':
    /* Parkes */
    x = -4554231.5;
    y = 2816759.1;
    z = -3454036.3;
    break;

  case '8':
    /* Jodrell Bank */
    x = 3822252.643;
    y = -153995.683;
    z = 5086051.443;
    break;

  case 'i':
    /* Westerbork, from:

       WSRT Site Information Form
       International GPS Service for Geodynamics
       http://igscb.jpl.nasa.gov/mail/igsmail/2000/msg00323.html

       NOTE: These values are only approximate

     */
    x = 3828739;
    y = 443305;
    z = 5064887;
    break;

  default:
    fprintf (stderr, "telescope_xyz: unknown telescope code %d\n",
	     telescope);
    return -1;
  }

  if (x_)
    *x_ = x;
  if (y_)
    *y_ = y;
  if (z_)
    *z_ = z;

  return 0;
}

