/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/coord.h,v $
   $Revision: 1.7 $
   $Date: 2003/12/23 01:06:22 $
   $Author: ahotan $ */

#ifndef __COORD_H
#define __COORD_H

#ifdef __cplusplus

#include "MJD.h"
 
/* ********************************************************************
   bary_earth -
        given an MJD, latitude and longitude in degrees, and elevation
        in metres, returns the barycentric velocity (fraction of c)
        and position (fraction of AU) of telescope.
   ******************************************************************** */
int bary_earth (const MJD& mjd_date, double longitude, double latitude,
                double elevation, double* baryvel, double* barypos);
 
/* ********************************************************************
   doppler_factor -
        given an MJD, then latitude, longitude and elevation (degrees
        and metres), and the ra and dec of an astronomical source
        (considered at rest), returns the doppler shift due to the
        telescope's barycentric velocity
   ******************************************************************** */
int doppler_factor (const MJD& date, double ra, double dec,
                    double longitude, double latitude, double elevation,
                    double* dfactor);
 
extern "C" {

#endif

/* ********************************************************************
   str2coord -
	converts a string with RA and DEC in their human notations:
	RA in time-measure (1h = 15 deg)
	DEC in sexagesimal angular measure
	into an ra and dec in radians
   ******************************************************************** */
  int str2coord (double *ra, double *dec, const char* coordstr);
  int str2ra  (double *ra,  const char* rastr);
  int str2dec (double *dec, const char* decstr);
  
  int coord2str (char* coordstring, unsigned strlen,
		 double ra, double dec, unsigned places);
  int ra2str (char* rastr, unsigned rastrlen, double ra, unsigned places);
  int dec2str (char* decstr, unsigned decstrlen, double dec, unsigned places);
  

  int coord2name (char* pulsar, double ra, double dec);

/* ********************************************************************
   telescope_coords -
	returns latitude and longitude (in degrees) and height
	(in metres) for telescope given by its tempo code
   ******************************************************************** */
int telescope_coords (char telescope, float* latitude, 
		      float* longitude, float* elevation);

/* ********************************************************************
   telescope_xyz -
        returns ITRF X, Y, X  coordinates (in metres) about the
        geocentre for a telescope given by its tempo code
   ******************************************************************** */
int telescope_xyz (char telescope, double* x_, 
		      double* y_, double* z_);

/* ********************************************************************
   az_zen_para -
        converts J2000 ra and dec (in radians), lst (in hours),
	and latitude (in degrees) to telescope azimuth and zenith
	angles and parallactic angle (in degrees)
   ******************************************************************** */
int az_zen_para (double ra, double dec, float lst, float latitude,
          float * tel_az, float * tel_zen, float * para_angle);

#ifdef __cplusplus
	   }
#endif

#endif
