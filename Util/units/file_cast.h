//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/file_cast.h,v $
   $Revision: 1.1 $
   $Date: 2009/05/27 06:10:32 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_FILE_CAST_H
#define __UTILS_UNITS_FILE_CAST_H

#include "FilePtr.h"

template<typename To, typename From>
To* file_cast (const From* from)
{
  FilePtr temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "file_cast", "tmpfile");
  
  from->unload (temp);
  rewind (temp);

  To* to = new To;
  to->load (temp);

  return to;
}

template<typename To, typename From>
To* dynamic_file_cast (From* from)
{
  To* to = dynamic_cast<To*>( from );
  if (to)
    return to;

  return file_cast<To>(from);
}

template<typename To, typename From>
const To* dynamic_file_cast (const From* from)
{
  const To* to = dynamic_cast<const To*>( from );
  if (to)
    return to;

  return file_cast<To>(from);
}

#endif
