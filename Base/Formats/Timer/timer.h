/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/timer.h,v $
   $Revision: 1.4 $
   $Date: 2000/01/31 05:50:55 $
   $Author: straten $ */

/* include file for the main timer structure                            */

/* This has to be compatable with fortran when written as binary to a file.
   It is *****ABSOLUTELY VITAL***** that eight byte boundaries are not crossed
   All characters should have a length equal to a multiple of 8 
   In any section of the structure (between character or real*8 declarations)
   there must be an even number of reals and integers */

/* If you cant do this with actual variables put in pads (eg pad1 below)
   Space is declared at the end to fill up to 1024 bytes. */

/* If you declare new variables you should subtract the appropriate no of bytes
   real = 4, integer = 4, real*8 = 8 etc on 32 bit architecture  */

/* Dont forget to alter version numbers here and in all programs using this 
   there is a list in header.uses */

#ifndef __TIMER_H
#define __TIMER_H

/* Polarization storage definitions */
#define IQ_POLN_STORAGE 1
#define XY_POLN_STORAGE 0

/* Obstype definitions */

#define HYDRA  20
#define NHYDRA 21
#define SHYDRA 22
#define PULSAR 0
#define CAL    1
#define SCAL   2 


/* A central place for the lengths of the character arrays */
#define RAM_BOARD_STRLEN 32
#define CORR_BOARDS_STRLEN 32
#define MACHINE_ID_STRLEN 8
#define UTDATE_STRLEN 16
#define COORD_TYPE_STRLEN 8
#define PSRNAME_STRLEN 16
#define TREE_STRLEN 8
#define TELID_STRLEN 16
#define TPOVER_STRLEN 8
#define SCHEDULE_STRLEN 32
#define COMMENT_STRLEN 64
#define CALFILE_STRLEN 24
#define SCALFILE_STRLEN 24
#define SOFTWARE_STRLEN 128
#define BACKEND_STRLEN 8
#define TLABEL_STRLEN 8
/* 
responsibilities
(T) timer
(G) get header
(A) Archiver
(C) Tcedar
(*) After archiver
*/

/*

NB! At the beginning on an observation each correlator dump
corresponds to a sub_int. It is only at the archiver stage
that we get several dumps becoming a sub_int.

*/
#include "environ.h"
#include "band.h"

struct timer {

/* Basics FPTM info     */

  char ram_boards[RAM_BOARD_STRLEN];	        /* (G) ID of ram adder boards                 */
  char corr_boards[CORR_BOARDS_STRLEN];	        /* (G) ID of correlator boards                */
  char machine_id[MACHINE_ID_STRLEN];           /* (T) Machine id Jose=1                      */
  float version;	        		/* (T) Timer version                          */
  float minorversion;           		/* (T) Minor version                          */
  int tape_number;	        		/* (*) Tape number of archiver                */
  int file_number;	        		/* (*) file number on tape                    */

/* Times and references */

  char utdate[UTDATE_STRLEN];		 	/* (G) dd-mm-yyyy                             */
  double fracmjd;               	 	/* (G) fractional part of mjd 0.0-0.99999     */
  int mjd;				 	/* (G) integer part of mjd                    */
  int number_of_ticks;			 	/* (G) Ticks between 1pps + int start         */
  double offset;	        	 	/* (G) diff between 10sec tick and start (ns) */
  double lst_start;			 	/* (A) seconds                                */

/* Pulsar Parameters    */

  char coord_type[COORD_TYPE_STRLEN];           /* (T) 02=Eq of date, 03=B, 04=Gal, 05=J    */
  char psrname[PSRNAME_STRLEN];	        	/* (T) Proper name                          */
  double ra;					/* (T) J2000 ra in radians                  */
  double dec;					/* (T) J2000 dec in radians                 */
  float l;					/* (T) Galactic latitude (deg)              */
  float b;					/* (T) Galactic longitude (deg)             */
  double nominal_period;        		/* (G/A) Nominal period of fold (s)         */
  float dm;					/* (A) dm                                   */
  int fold_true_ratio;				/* (G/A) Periods across bins 2^nn=0,1...    */
  int nperiods_long;				/* (G/A) periods in long int                */
  int nperiods_short;				/* (G/A) periods in short int               */

/* Observation Configuration */

  int   nbin;					/* (T/A) bins in profile                   */
  float tsmp;                   		/* (G) sampling interval                   */
  float sub_int_time;	        		/* (T/A) time for one sub_int              */
  int   ndump_sub_int;          		/* (T/A) dumps per sub_int (1 in timer)    */
  int   narchive_int;				/* (T) archives per integration            */
  int   junk;	                		/* reserved                                */
  int   nsub_int;               		/* (T/A) number of sub_ints in an archive 
						   at the timer stage this is ndumps in
						   an archive                        */
  int   junk2;                  		/* reserved                                 */
  float dump_time;              		/* (T) time for a dump                      */
  int   nfreq;					/* (T/A) No of RFs (1 or 2)                 */
  int   nsub_band;              		/* (A) No of subands across badndwidth      */
  int   feedmode;               		/* (T) 0=const feed angle 1=const pos angle */
  char  tree[TREE_STRLEN];                      /* (T) ' ' normal 'T' tree 'D' pdm desip    */  
  char  telid[TELID_STRLEN];                    /* (T) Telescope id (as for tel.def)        */

/* Polyco */

  char tpover[TPOVER_STRLEN];                   /* (T) tempo version                        */
  int  nspan;					/* (T) minutes polyco for                   */
  int  ncoeff;					/* (T) coefficients in polyco               */
  int  nbytespoly;              		/* (A) nbytes in polyco                     */
  int  nbytesephem;             		/* (A) nbytes in ephemeris                  */

/* Details of each band */

  struct band banda;				/* details of band a */
  struct band bandb;				/*    "    "   "   b */
		
/* Misc */		  
		
  float rotm;                   		/* Catalogue RM */
  float rmi;                    		/* Ionospheric RM */
  float pnterr;                 		/* (T) pointing error in arcseconds */
  int ibeam;                    		/* (T) Beam # */
  char  tape_label[TLABEL_STRLEN];              /* Tape label                       */

/* General info and afterthoughts */

  char  schedule[SCHEDULE_STRLEN];   /* (T) schedule file used                     */
  char  comment[COMMENT_STRLEN];     /* (T) comment in schedule file               */
  float pos_angle;                   /* (T?) ?? */
  int   headerlength;                /* (T)  ?? */
  int   corrected;                   /* (A) 1 if archive is corrected for all bugs */
  int   calibrated;                  /* (C) 1 if archive is calibrated             */
  int   obstype;                     /* (T) see top of this file for list          */
  char  calfile[CALFILE_STRLEN];     /* (C) cal archive used for calibration       */
  char  scalfile[SCALFILE_STRLEN];   /* (C) scal archive used for calibration      */
  int   wts_and_bpass;               /* (A) 1 if weights and bandpass are present  */
  int   wtscheme;                    /* (C) weighting scheme used in tcedar        */

  char  software[SOFTWARE_STRLEN];   /* info on software that produced archive     */

  char   backend [BACKEND_STRLEN];   /* 8-byte code checked for recognized backend */
  uint32 be_data_size;            /* if backend is recognized, this value is
				     interpreted by archive::load as the number
				     of bytes to skip immediately following the
				     end of the timer header.  It is in this
				     space that derived classes of archive
				     should put additional information.
				     DEFINE recognized backend codes in
				     timer++.C */

#if defined (sun) && defined(__i386)
#define TIMER_SPACE 192
#else
#define TIMER_SPACE 188
#endif

  /* NB! SPACE IS ALWAYS THE LAST THING */
  char space[TIMER_SPACE];

};

#endif
