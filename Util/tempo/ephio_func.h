/* ADVICE!:  Do not include this file directly.  Instead, #include "ephio.h"

   $Source: /cvsroot/psrchive/psrchive/Util/tempo/ephio_func.h,v $
   $Revision: 1.1 $
   $Date: 2000/01/06 01:22:34 $
   $Author: straten $ */

#ifndef __EPHIO_FUNC_H
#define __EPHIO_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif
  
  extern int rd_eph (char* filename, int* parmStatus,
		     char strs[EPH_NUM_KEYS][EPH_STR_LEN],
		     double* value, int* integer, double* error);
  
  extern int rd_eph_lun (int lun, int* parmStatus,
			 char strs[EPH_NUM_KEYS][EPH_STR_LEN],
			 double* value, int* integer, double* error);
  
  extern int wr_eph (char* filename, int* parmStatus,
		     char strs[EPH_NUM_KEYS][EPH_STR_LEN],
		     double* value, int* integer, double* error);
  
  extern int wr_eph_lun (int lun, int* parmStatus,
			 char strs[EPH_NUM_KEYS][EPH_STR_LEN],
			 double* value, int* integer, double* error);
  
  /* ***********************************************************************
     rd_eph_str - parse from a string
     
     Use this function to parse one string at a time.
     As with rd_eph*, the user must supply arrays of size EPH_NUM_KEYS.
     This function will parse the values into the appropriate element of the
     arrays.
     
     The user must initialize the arrays before repeatedly calling this routine
     and call 'convertUnits_' to correct the units after completion
  */
  int rd_eph_str (int* parmStatus,
		  char* strs,
		  double* value, int* integer,
		  double* error, int* convert,
		  int* isOldEphem, char* parsethis);
  
  int convertunits_ (double* value, double* error, int* status, int* convert);
  
  /* writes a string into buffer that could be parsed by rd_eph_str */
  int wr_eph_str (char *buffer, int buflen, int ephind, int parmStatus, 
		  char* value_str, double value, int integer, double error);

  extern int length_(char *, int);  /* Fortan string length, in ephio.f */


#ifdef __cplusplus
	   }
#endif
  
#endif
