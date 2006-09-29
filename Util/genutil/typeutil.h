//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/typeutil.h,v $
   $Revision: 1.5 $
   $Date: 2006/09/29 14:13:06 $
   $Author: straten $ */

#ifndef __typeutil_h
#define __typeutil_h

#include "Reference.h"
#include <typeinfo>

template<class T>
unsigned find (const std::vector< Reference::To<T> >& array, const T* instance)
{
  unsigned index;

  for (index=0; index<array.size(); index++)
    if ( typeid(*instance) == typeid(*array[index].ptr()) )
      break;

  return index;
}

#endif

