#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "coord.h"
#include "f772c.h"

/*! Parses a string containing RA/DEC coordinates to produce ra and
  dec in radians.  The string may be of the form:

	       hhmm~ddmm    or  hhmmss~ddmmss
	       hh:mm~dd:mm  or  hh:mm:ss~dd:mm:ss

   or many other combinations, as long as:

   1) each field contains exactly two characters 
   2) text delimits a maximum of two characters (may contain one, so
      h:mm:ss works)

	       hh  hour angle of ra
	       dd  degrees of declination
	       mm  minutes
	       ss  seconds
	       ~   + or - sign of the degrees of declination

   returns a -1 on error, zero on success */
int str2coord (double *ra, double *dec, const char* coordstring) 
{
  int retval = 0;

  char* coordstr = strdup (coordstring);
  char* rastr = coordstr;   /* beginning of RA */

  /* search for the +/- sign of the DEC */
  char* decstr = strchr (coordstr, '-');
  if (decstr == NULL)
    decstr = strchr (coordstr, '+');
  if (decstr == NULL)
    decstr = strchr (coordstr, ' ');
  if (decstr == NULL)
    decstr = strchr (coordstr, '\t');
  if (decstr == NULL) {
    fprintf (stderr, "str2coord: No +,-,<space>, or <tab> in '%s'\n",coordstr);
    free (coordstr);
    return -1;
  }

  // fprintf (stderr, "str2coord: decstr='%s'\n", decstr);
  if (str2dec (dec, decstr) < 0)
    retval = -1;

  *decstr = '\0';

  // fprintf (stderr, "str2coord: rastr='%s'\n", rastr);
  if (str2ra (ra, rastr) < 0)
    retval = -1;

  free (coordstr);

  return retval;
}


/*! parses a string containing fields into a unit value.

  This function is particularly useful for parsing strings of the form
  hh:mm:ss.sssss or dd:mm:ss.ssss

  \retval unit        value parsed from the string
  \retval nfields     the number of fields in the string
  \retval field_width widths (number of characters) of each field
  \retval field_scale values with which to normalize the value in each field
  \param  unit_string string containing the value to be parsed

  If the field_width for a given field is equal to zero, that field is
  treated as a floating point number of an undetermined width (number of
  characters).  Otherwise, only field_width characters will be parsed.

  The number parsed from each field will be normalized by a scale factor
  that grows with each field read.  ie. for the i'th field, the scale factor
  will be equal to field_scale[i]*field_scale[i-1]*...*field_scale[0].

  \return the number of fields parsed from the string. */
int str2unit (double* unit, unsigned nfields,
	      const int* field_width, const double* field_scale,
	      const char* unit_string)
{
  char* unit_string_copy = strdup (unit_string);
  char* field_string_copy = strdup (unit_string);

  char* curstr = unit_string_copy;
  char* endstr = strchr (curstr, '\0');

  double field_value = 0;

  double sign = 1.0;
  double value = 0.0;
  double scale = 1.0;

  unsigned ifield;
  
  /* skip leading non-numeric characters */
  while ( curstr<endstr && !isdigit(*curstr) 
	  && *curstr != '-' && *curstr != '+' )
    curstr++;

  if (*curstr == '-') {
    sign = -1.0;
    curstr ++;
  }
  else if (*curstr == '+') {
    sign = 1;
    curstr ++;
  }

  for (ifield=0; curstr<endstr && ifield<nfields; ifield++) {

    /* skip leading non-numeric characters */
    while ( curstr<endstr && !isdigit(*curstr) )
      curstr++;

    if (field_width[ifield]) {
      strncpy (field_string_copy, curstr, field_width[ifield]);
      field_string_copy [field_width[ifield]] = '\0';
      curstr += field_width[ifield];
    }
    else {
      strcpy (field_string_copy, curstr);
      /* skip to end of numeric characters */
      while ( curstr<endstr && (isdigit(*curstr) || *curstr=='.') )
	curstr++;
    }

    // fprintf (stderr, "str2unit: field[%d]=%s\n", ifield, field_string_copy);

    if (sscanf (field_string_copy, "%lf", &field_value) != 1)
      break;

    scale /= field_scale[ifield];
    value += sign * scale * field_value;

  }

  free (field_string_copy);
  free (unit_string_copy);

  *unit = value;
  return ifield;

} 

int str2ra (double *ra, const char* rastring) 
{
  int field_width[3] = {2, 2, 0};
  double field_scale[3] = {24.0, 60.0, 60.0};

  if (str2unit (ra, 3, field_width, field_scale, rastring) < 0)
    return -1;

  *ra *= 2.0 * M_PI;
  return 0;
}

int str2dec (double *dec, const char* decstring) 
{
  int field_width[3] = {2, 2, 0};
  double field_scale[3] = {360.0, 60.0, 60.0};

  if (str2unit (dec, 3, field_width, field_scale, decstring) < 0)
    return -1;

  *dec *= 2.0 * M_PI;
  return 0;
}


/*! Produces a string of the form hh:mm:ss.sss[+|-]dd:mm:ss.sss,
  given ra and dec in radians.  The number of decimal places in ss.sss
  is controlled by places.  */
int coord2str (char* coordstring, unsigned coordstrlen, double ra, double dec,
	       unsigned places) 
{
  int decstart;

  ra2str (coordstring, coordstrlen, ra, places);

  decstart = strlen (coordstring);

  if (dec >= 0) {
    coordstring[decstart] = '+';
    decstart ++;
  }

  dec2str (coordstring+decstart, coordstrlen-decstart, dec, places);

  return 0;
}

/*! returns the number of fields successfully parsed into unit_string. */
int unit2str (char* unit_string, unsigned unit_strlen,
	      unsigned nfields,
	      const int* field_width, const int* field_precision,
	      const double* field_scale, char separator, double unit)
{
  char* end_string = unit_string + unit_strlen;

  double field_value = 0;

  unsigned ifield;
  int printed = 0;

  if (unit_strlen < 1)
    return 0;

  if (unit < 0) {
    unit *= -1.0;
    *unit_string = '-';
    unit_string ++;
  }

  for (ifield=0; unit_string<end_string && ifield<nfields; ifield++) {

    if (separator && ifield > 0) {
      *unit_string = separator;
      unit_string ++;
    }

    unit *= field_scale[ifield];

    if (field_precision[ifield]) {
      field_value = unit;
      printed = snprintf (unit_string, end_string-unit_string, "%0*.*f",
			  field_width[ifield]+field_precision[ifield]+1,
			  field_precision[ifield], field_value);
    }
    else {

      // trick to avoid rounding ugliness in string
      if (ifield+1 < nfields && field_precision[ifield+1]) {
	field_value = pow (10.0,-(field_precision[ifield+1]+3));
	unit += field_value/field_scale[ifield+1];
      }

      field_value = floor (unit);
      printed = snprintf (unit_string, end_string-unit_string, "%.*d",
			  field_width[ifield], (int) field_value);
    }
    
    unit -= field_value;

    if (printed < 0)
      return ifield;

    unit_string += printed;

  }

  return ifield;
} 

/* given a value in radians, returns a string parsed into either
   hh:mm:ss.sss or (-)dd:mm:ss.sss format, as determined by setting
   scale equal to either 24 or 360, respectively */
int xms2str (char* xms_str, unsigned xms_strlen, double radians,
	     double scale, unsigned places) 
{
  int field_width[3] = {2, 2, 2};
  int field_precision[3] = {0, 0, 0};
  double field_scale[3] = {0.0, 60.0, 60.0};

  field_precision[2] = places;
  field_scale[0] = scale;

  radians /= 2.0 * M_PI;
  return unit2str (xms_str, xms_strlen, 3, field_width, field_precision,
		   field_scale, ':', radians);
}

int ra2str (char* rastring, unsigned rastrlen, double ra, unsigned places) 
{
  return xms2str (rastring, rastrlen, ra, 24.0, places);
}

int dec2str (char* dstring, unsigned dstrlen, double dec, unsigned places) 
{
  return xms2str (dstring, dstrlen, dec, 360.0, places);
}




/* takes an RA and DEC in radians and converts it to an hour angle and
   degree format, then outputs this in a 13 digit string */
int coord2name (char* pulsar, double ra, double dec)
{
  int hr, rmin, rsec, deg, dmin, dsec;
  int isign = dec / fabs(dec);
  char sign = (isign > 0)?'+':'-';

  dec *= 180.0 / M_PI;
  ra  *= 12.0 / M_PI;

  hr = (int) ra;
  ra -= (double) hr;
  ra *= 60;
  rmin = (int) ra;
  ra -= (double) rmin;
  ra *= 60;
  rsec = (int) ra;

  dec = fabs (dec);

  deg = (int) dec;
  dec -= (double) deg;
  dec *= 60;
  dmin = (int) dec;
  dec -= (double) dmin;
  dec *= 60;
  dsec = (int) dec;

  sprintf (pulsar, "%02d%02d%02d%c%02d%02d%02d", hr,rmin,rsec, sign,
	   deg,dmin,dsec);
  return 0;
}

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
     telescope = '0' + telescope;

  switch (telescope) {

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

/* **********************************************************************

   az_zen_para - given the J2000 ra and dec, local sidereal time, and
                 latitude, returns the azimuth, zenith, and parallactic
		 angles of the telescope.

		 ra and dec are given in radians
		 lst is given in hours
		 latitude is given in degrees

		 para_angle, tel_az and tel_zen are returned in degrees

   Uses: SLALIB SUBROUTINE sla_ALTAZ (HA, DEC, PHI,
                           AZ, AZD, AZDD, EL, ELD, ELDD, PA, PAD, PADD)

   ********************************************************************** */

void F772C(sla_altaz) (double*, double*, double*, double*, double*, double*,
		 double*, double*, double*, double*, double*, double*);

int az_zen_para (double ra, double dec, float lst, float latitude,
		 float* tel_az, float* tel_zen, float* para_angle)
{
  double altitude, azimuth, PA;
  double ignore;

  double HA = lst * M_PI/12.0 - ra;     /* hour angle */
  double rad2deg = 180.0 / M_PI;
  double dlat = latitude / rad2deg;

  if (lst < 0.0 || lst > 24.0)
    return -1;

  F772C(sla_altaz) (&HA, &dec, &dlat, 
	      &azimuth, &ignore, &ignore,
	      &altitude,  &ignore, &ignore,
	      &PA,      &ignore, &ignore);

  *para_angle = (float) PA  * rad2deg;
  *tel_zen = 90.0 - (float) altitude * rad2deg;
  *tel_az = (float) azimuth * rad2deg;

  return 0;
}

