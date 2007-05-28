/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "ReferenceAble.h"

using namespace std;

#if _DEBUG
#include <iostream>
#endif

// static ensures read-only access via get_instance_count
static uint64_t instance_count = 0;

uint64_t Reference::Able::get_instance_count ()
{
  return instance_count;
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able ()
{
  instance_count++;
  __reference_count = 0;
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able (const Able&)
{
  instance_count++;
  __reference_count = 0;
}

template<class C>
void clean (C& c)
{
  typename C::iterator it = c.begin();
  while (it != c.end()) {
    *(*it) = 0;
    it ++;
  }
  c.resize(0);
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::~Able ()
{ 
  instance_count--;

  clean (__reference_list);

#ifdef _DEBUG
  cerr << "Reference::Able::~Able exit" <<endl;
#endif

}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const T> */
void Reference::Able::__add_reference (bool active, Able** ref_address) const
{

#ifdef _DEBUG
  cerr << "Reference::Able::__add_reference ptr=" << ref_address 
       << " this=" << this << " active=" << active << endl;
#endif

  // function is declared const, but __reference_list must be modified
  Able* thiz = const_cast<Able*> (this);

  thiz->__is_on_heap();
  thiz->__reference_list.push_back (ref_address);

  if (active)
    thiz->__reference_count ++;

#ifdef _DEBUG
  cerr << "Reference::Able::__add_reference count="
       << __reference_count << endl;
#endif

}

template<class C, class T>
void erase (C& c, T t)
{
  for (typename C::iterator it = c.begin(); it != c.end(); it++)
    if (*it == t) {
      c.erase(it);
      break;
    }
}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const Klass> */
void Reference::Able::__remove_reference (bool active, Able** ref_address,
					  bool auto_delete) const
{ 

#ifdef _DEBUG
  cerr << "Reference::Able::__remove_reference=" << ref_address 
       << " this=" << this << " active=" << active << endl;
#endif

  // function is declared const, but __reference_list must be modified
  Able* thiz = const_cast<Able*> (this);

  erase (thiz->__reference_list, ref_address);

  if (active)  {
    // assert (__reference_count > 0);
    thiz->__reference_count --;
  }

#ifdef _DEBUG
  cerr << "Reference::Able::__remove_reference count="
       << __reference_count << endl;
#endif

  // delete when reference count reaches zero and instance is on heap
  if ( active && auto_delete && __reference_count == 0 && __is_on_heap() ) {

#ifdef _DEBUG
    cerr << "Reference::Able::__remove_reference delete this=" << this << endl;
#endif
    thiz->__heap_state = 0x02;

    clean(thiz->__reference_list);

    delete this;

  }

}
