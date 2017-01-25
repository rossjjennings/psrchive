//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/typeutil.h

#ifndef __typeutil_h
#define __typeutil_h

#include "Reference.h"
#include <typeinfo>

using namespace std;

template<class T>
unsigned find (const vector< Reference::To<T> >& array, const T* instance)
try 
{
  unsigned index;

  if (!instance)
    throw Error (InvalidParam, "typeinfo find", "null pointer to instance");

#ifdef _DEBUG
  cerr << "typeinfo find array size = " << array.size() 
       << " ptr=" << instance 
       << " name=" << instance->get_extension_name() << endl;
#endif

  for (index=0; index<array.size(); index++)
  {
#ifdef _DEBUG
    cerr << "typeinfo find test index=" << index 
         << " ptr=" << array[index].ptr();
    cerr << " name=" << array[index]->get_extension_name () << endl;
#endif
    if ( typeid(*instance) == typeid(*array[index].get()) )
      break;
  }

#ifdef _DEBUG
  cerr << "typinfo find return index = " << index << endl;
#endif

  return index;
}
catch (Error& error)
{
  throw error += "typeutil find";
}

#endif

