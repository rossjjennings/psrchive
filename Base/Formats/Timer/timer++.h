/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/timer++.h,v $
   $Revision: 1.3 $
   $Date: 1999/11/18 05:22:34 $
   $Author: straten $ */

#ifndef __TIMER_PLUSPLUS_H
#define __TIMER_PLUSPLUS_H

// ////////////////////////////////////////////////////////////////////
// C++ methods that perform operations on the timer struct
// or calculations with the timer struct
//
// call these functions like:  Timer::load, etc.
//
// ////////////////////////////////////////////////////////////////////
 
#include <stdio.h>
#include "timer.h"
#include "MJD.h"
#include "psrnamespace.h"

NAMESPACE (Timer)

     static int  nbackends;
     static char backends[][BACKEND_STRLEN+1];

     // returns the size of the additional backend information, or 0 if none
     NMSPCF unsigned long backend_data_size(const struct timer& hdr);

     // returns index of code in 'backends' or -1 if not recognized
     NMSPCF int backend_recognized (const char* backend);

     // sets the name of the backend in 'hdr' to 'backend'
     // returns 0 if successful, -1 otherwise
     NMSPCF int set_backend (struct timer* hdr, const char* backend);

     // returns a string containing the backend code, or "un-recognized" if so
     NMSPCF string get_backend (const struct timer& hdr);

  // may be used to receive soft error messages
  static string reason;

  // loads a timer struct, givent a file name
  NMSPCF int load (const char* fname, struct timer* hdr, bool big_endian=true);

  // loads a timer struct from a file pointer
  NMSPCF int load (FILE* fptr, struct timer* hdr, bool big_endian=true);

  // unloads a timer struct to a file (always big endian)
  NMSPCF int unload (FILE* fptr, struct timer& hdr);

  // returns the MJD from a timer struct
  NMSPCF MJD get_MJD (const struct timer& hdr);

  // sets the MJD in a mini struct
  NMSPCF void set_MJD (struct timer& hdr, const MJD& mjd);

  // returns the poln_storage code from a timer struct
  NMSPCF int poln_storage (const struct timer& hdr);

  // returns true if two archives can be mixed (tadded, for instance)
  // set max_freq_sep if you wish to alter the action of this function
  NMSPCF bool mixable (const timer& hdr1, const timer& arch2,
		       double max_freq_sep = 0.10 /* 100kHz */);

  // a function to set most of timer struct to NULL values.
  NMSPCF void init (struct timer * hdr);

END_NAMESPACE

#endif // __TIMER_PLUSPLUS_H
