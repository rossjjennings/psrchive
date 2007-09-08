/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HeapTracked.h"
#include "Error.h"

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

// #define _DEBUG 1

using namespace std;

// static ensures read-only access via get_heap_queue_size
static vector<const void*> heap_addresses;

#ifdef HAVE_PTHREAD
static pthread_mutex_t heap_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

size_t Reference::HeapTracked::get_heap_queue_size ()
{
  return heap_addresses.size();
}

bool Reference::verbose = false;

void* Reference::HeapTracked::operator new (size_t size, void* ptr)
{
  if (ptr) {

#ifdef _DEBUG
    cerr << "Reference::HeapTracked::operator placement new size=" << size 
	 << " ptr=" << ptr << endl;
#endif

    return ptr;
  }
  
#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&heap_mutex);
#endif

  ptr = ::operator new (size);

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::operator new size=" << size 
       << " ptr=" << ptr << endl;
#endif

  heap_addresses.push_back (ptr);

#if 0
  static unsigned max_size = 0;
  if (heap_addresses.size() > max_size) {
    max_size = heap_addresses.size();
    cerr << "max. size=" << heap_addresses.size() << endl;
  }
#endif

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&heap_mutex);
#endif

  return ptr;
}

void Reference::HeapTracked::operator delete (void* ptr)
{
#ifdef _DEBUG
  cerr << "Reference::HeapTracked::operator delete void*=" << ptr << endl;
#endif

  ::operator delete (ptr);
}

bool Reference::HeapTracked::__is_on_heap () const
{
  return const_cast<HeapTracked*>(this)->__is_on_heap ();
}

bool Reference::HeapTracked::__is_on_heap ()
{
  const char is_on_heap = 0x01;

  if (__heap_state) {

#ifdef _DEBUG
    cerr << "Reference::HeapTracked::__is_on_heap __heap_state="
         << (int)__heap_state << endl;
#endif

    return __heap_state == is_on_heap;
  }

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap this=" << this << endl;
#endif

  const void* raw_address = dynamic_cast<const void*>(this);

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap void*=" 
       << raw_address << endl;
#endif

  if (raw_address == NULL)
    throw Error (InvalidPointer, "Reference::HeapTracked::is_on_heap",
		 "failed dynamic_cast<const void*>(%p)", this);
  
  vector<const void*>::iterator it;

#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&heap_mutex);
#endif

  for (it = heap_addresses.begin(); it != heap_addresses.end(); it++) {

#ifdef _DEBUG
    unsigned long itl = (unsigned long) *it;
    unsigned long ral = (unsigned long) raw_address;

    cerr << "Reference::HeapTracked::is_on_heap heap address=" << *it
	 << " diff=" << (ral - itl) << endl;
#endif

    if (*it == raw_address)
      break;
  }

  if ( it != heap_addresses.end() ) {

    heap_addresses.erase (it);
    __heap_state = is_on_heap;

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap true heap_state="
         << (int)__heap_state << endl;
#endif

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&heap_mutex);
#endif

    return true;
  }

  __heap_state = 0x04;

#ifdef _DEBUG
    cerr << "Reference::HeapTracked::is_on_heap false heap_state="
         << (int)__heap_state << endl;
#endif

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&heap_mutex);
#endif

  return false;
}
