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

END_NAMESPACE

#endif // __TIMER_PLUSPLUS_H
