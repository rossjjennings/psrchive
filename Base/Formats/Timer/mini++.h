/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/mini++.h,v $
   $Revision: 1.2 $
   $Date: 1999/11/02 09:58:22 $
   $Author: straten $ */

#ifndef __MINI_PLUSPLUS_H
#define __MINI_PLUSPLUS_H

// ////////////////////////////////////////////////////////////////////
// C++ methods that perform operations on the mini struct
// or calculations with the mini struct
//
// call these functions like:  Mini::load (), etc.
//
// ////////////////////////////////////////////////////////////////////
 
#include <stdio.h>
#include "mini.h"
#include "MJD.h"
#include "psrnamespace.h"

NAMESPACE (Mini)

  // may be used to receive soft error messages
  static string reason;

  // loads a mini struct, givent a file name
  NMSPCF int load (const char* fname, struct mini* hdr, bool big_endian=true);

  // loads a mini struct from a file pointer
  NMSPCF int load (FILE* fptr, struct mini* hdr, bool big_endian=true);

  // unloads a mini struct to a file (always big endian)
  NMSPCF int unload (FILE* fptr, struct mini& hdr);

  // returns the MJD from a mini struct
  NMSPCF MJD get_MJD (const struct mini& hdr);

  // sets the MJD in a mini struct
  NMSPCF void set_MJD (struct mini& hdr, const MJD& mjd);

END_NAMESPACE

#endif // __MINI_PLUSPLUS_H
