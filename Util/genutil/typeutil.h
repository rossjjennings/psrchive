//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/typeutil.h,v $
   $Revision: 1.2 $
   $Date: 2003/10/08 13:20:08 $
   $Author: straten $ */

#ifndef __typeutil_h
#define __typeutil_h

#include <typeinfo>

//! Find the instance with the specified type_info
template<typename T>
unsigned find (const vector<T>& exts, const type_info& info)  {
  
  unsigned index = 0;
  
  while (index < exts.size())  {
    if ( typeid(exts[index]) == info )
      break;
    index ++;
  }
  
  return index;
  
}

#endif
