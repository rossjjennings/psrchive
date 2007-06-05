/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "ReferenceAble.h"
#include <assert.h>

using namespace std;

#if _DEBUG
#include <iostream>
#endif

// static ensures read-only access via get_instance_count
static size_t instance_count = 0;

size_t Reference::Able::get_instance_count ()
{
  return instance_count;
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able ()
{
  instance_count++;
  __reference_count = 0;
  __reference_handle = 0;

#ifdef _DEBUG
  cerr << "Reference::Able ctor this=" << this
       << " instances=" << instance_count << endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able (const Able&)
{
  instance_count++;
  __reference_count = 0;
  __reference_handle = 0;

#ifdef _DEBUG
  cerr << "Reference::Able copy ctor this=" << this
       << " instances=" << instance_count << endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::~Able ()
{ 
  instance_count--;

  if (__reference_handle)
    __reference_handle->pointer = 0;

#ifdef _DEBUG
  cerr << "Reference::Able::~Able instances=" << instance_count <<endl;
#endif

}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const T> */
Reference::Able::Handle*
Reference::Able::__reference (bool active) const
{

#ifdef _DEBUG
  cerr << "Reference::Able::__reference this=" << this 
       << " active=" << active << endl;
#endif

  // function is declared const, but private attributes are modified
  Able* thiz = const_cast<Able*> (this);

  thiz->__is_on_heap();

  if (!__reference_handle) {
#ifdef _DEBUG
  cerr << "Reference::Able::__reference new handle" << endl;
#endif
    thiz->__reference_handle = new Handle;
    thiz->__reference_handle->pointer = thiz;
  }

  if (active)
    thiz->__reference_count ++;

#ifdef _DEBUG
  cerr << "Reference::Able::__reference count="
       << __reference_count << endl;
#endif

  return thiz->__reference_handle;
}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const Klass> */
void Reference::Able::__dereference (bool auto_delete) const
{ 
  assert (__reference_count > 0);

  // function is declared const, but __reference_list must be modified
  Able* thiz = const_cast<Able*> (this);

  thiz->__reference_count --;

#ifdef _DEBUG
  cerr << "Reference::Able::__dereference this=" << this
       << " count=" << __reference_count << endl;
#endif

  // delete when reference count reaches zero and instance is on heap
  if ( auto_delete && __reference_count == 0 && __is_on_heap() ) {

#ifdef _DEBUG
    cerr << "Reference::Able::__dereference delete this=" << this << endl;
#endif

    thiz->__heap_state = 0x02;

    if (__reference_handle)
      thiz->__reference_handle->pointer = 0;

    delete this;

  }

}
