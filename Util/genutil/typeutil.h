//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/typeutil.h,v $
   $Revision: 1.3 $
   $Date: 2004/12/15 14:36:31 $
   $Author: straten $ */

#ifndef __typeutil_h
#define __typeutil_h

#include "Reference.h"
#include <typeinfo>

template<class T>
unsigned find (const vector< Reference::To<T> >& array, const T* instance)
{
  unsigned index;

  for (index=0; index<array.size(); index++)
    if ( typeid(*instance) == typeid(*array[index].ptr()) )
      break;

  return index;
}

#endif

