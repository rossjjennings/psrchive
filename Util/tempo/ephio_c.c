/* This file defines C wrappers to the fortran rd_eph and wr_eph. */

#include "ephio.h"
#include <string.h>
#include <stdlib.h>

#if 0
/* extern "C" {   **** include this for C++ compilers */
int length_(char *, int);  /* Fortan string length, in ephio.f */

int rd_eph_(char *, int *, char *, double *,
	    int *, double *, long int, long int);
int wr_eph_(char *, int *, char *, double *,
	    int *, double *, long int, long int);
int rd_eph_lun_(int *, int *, char *, double *,
	    int *, double *, long int);
int wr_eph_lun_(int *, int *, char *, double *,
	    int *, double *, long int);
/*}*/
#endif

int rd_eph_wrap (int uselun, char *fname, int lun, 
		 int parmStatus[EPH_NUM_KEYS], 
		 char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
		 double value_double[EPH_NUM_KEYS], 
		 int value_integer[EPH_NUM_KEYS],
		 double error_double[EPH_NUM_KEYS])
{
  char v_str[EPH_NUM_KEYS*EPH_STR_LEN];
  int i,j, retval;

  if (uselun)
    retval = rd_eph_lun__(&lun, parmStatus, v_str, value_double, value_integer,
		     error_double, EPH_STR_LEN);
  else
    retval = rd_eph__(fname, parmStatus, v_str, value_double, value_integer,
		     error_double, strlen(fname), EPH_STR_LEN);

  /* fix up the 2-d array. If C compilers would stick to standards we
     could just column-major to row-major swap but these days compilers
     seem to be using char x[A][B] --> char ** x. 
     Also we need to convert the strings to '\0' terminated */
  for (i=0; i < EPH_NUM_KEYS; i++)
  {
    if (parmStatus[i])
    {
      j = length_(v_str+i*EPH_STR_LEN, EPH_STR_LEN);
      memcpy(value_str[i], v_str+i*EPH_STR_LEN, j);
      value_str[i][j] = '\0';
    }
  }

  return retval;
}

int rd_eph(char *fname, int parmStatus[EPH_NUM_KEYS], 
	   char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
	   double value_double[EPH_NUM_KEYS], int value_integer[EPH_NUM_KEYS],
	   double error_double[EPH_NUM_KEYS])
{
  return rd_eph_wrap(0, fname, 0, parmStatus, value_str,
		     value_double, value_integer, error_double);
}

int rd_eph_lun(int lun, int parmStatus[EPH_NUM_KEYS], 
	       char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
	       double value_double[EPH_NUM_KEYS],
	       int value_integer[EPH_NUM_KEYS],
	       double error_double[EPH_NUM_KEYS])
{
  return rd_eph_wrap(1, NULL, lun, parmStatus, value_str,
		     value_double, value_integer, error_double);
}


int wr_eph_wrap(int uselun, char *fname, int lun,
		int parmStatus[EPH_NUM_KEYS], 
		char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
		double value_double[EPH_NUM_KEYS],
		int value_integer[EPH_NUM_KEYS],
		double error_double[EPH_NUM_KEYS])
{
  char v_str[EPH_NUM_KEYS*EPH_STR_LEN];
  int i,j, retval;

  /* fix up the 2-d array. */
  for (i=0; i < EPH_NUM_KEYS; i++)
  {
    if ((parmTypes[i]==0)&&(parmStatus[i]))
    {
      j = strlen(value_str[i]);
      memcpy(v_str+i*EPH_STR_LEN, value_str[i], j);
      for ( ; j < EPH_STR_LEN; j++)
	*(v_str+i*EPH_STR_LEN+j)= ' ';	/* fortran space-padded string */
    }
  }

  if (uselun)
    retval = wr_eph_lun__(&lun, parmStatus, v_str, value_double, value_integer,
		   error_double, EPH_STR_LEN);
  else
    retval = wr_eph__(fname, parmStatus, v_str, value_double, value_integer,
		   error_double, strlen(fname), EPH_STR_LEN);

  return retval;
}

int wr_eph(char *fname, int parmStatus[EPH_NUM_KEYS], 
	   char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
	   double value_double[EPH_NUM_KEYS],
	   int value_integer[EPH_NUM_KEYS],
	   double error_double[EPH_NUM_KEYS])
{
  return wr_eph_wrap(0, fname, 0, parmStatus, value_str,
		     value_double, value_integer, error_double);
}

int wr_eph_lun(int lun,
	       int parmStatus[EPH_NUM_KEYS], 
	       char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
	       double value_double[EPH_NUM_KEYS],
	       int value_integer[EPH_NUM_KEYS],
	       double error_double[EPH_NUM_KEYS])
{
  return wr_eph_wrap(1, NULL, lun, parmStatus, value_str,
		     value_double, value_integer, error_double);
}
