//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/Telescope.h,v $
   $Revision: 1.7 $
   $Date: 2007/06/21 17:32:28 $
   $Author: straten $ */

#ifndef __Telescope_h
#define __Telescope_h

#include <string>

//! Tempo telescope codes
namespace Telescope {

  const char Parkes = '7';
  const char ATCA = '2';
  const char Tidbinbilla = '6';
  const char Arecibo = '3';
  const char Hobart = '4';
  const char Greenbank = '1';
  const char WSRT = 'i';
  const char AAT = '5';

  //! Convert a telescope name to a code
  char code (const std::string& telescope_name);

  //! Convert a telecope code to a name
  const char* name (char code);

}

#endif
