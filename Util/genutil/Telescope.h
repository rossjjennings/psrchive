//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/Telescope.h,v $
   $Revision: 1.1 $
   $Date: 2003/10/06 13:11:26 $
   $Author: straten $ */

#ifndef __Telescope_h
#define __Telescope_h

//! Tempo telescope codes
namespace Telescope {

  const char Parkes = '7';
  const char ATCA = '2';
  const char Tidbinbilla = '6';
  const char Arecibo = '3';
  const char Hobart = '4';

  //! Convert a telescope name to a code
  char code (const char* telescope_name);

  //! Convert a telecope code to a name
  const char* name (char code);

}

#endif
