#include "HeapTracked.h"
#include "Error.h"

// #define _DEBUG 1

vector<const void*> Reference::HeapTracked::__heap_addresses;

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
  
  ptr = ::operator new (size);

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::operator new size=" << size 
       << " ptr=" << ptr << endl;
#endif

  __heap_addresses.push_back (ptr);

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

  const void* raw_address = dynamic_cast<const void*>(this);

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap this=" << this << " void*=" 
       << raw_address << endl;
#endif

  if (raw_address == NULL)
    throw Error (InvalidPointer, "Reference::HeapTracked::is_on_heap",
		 "failed dynamic_cast<const void*>(%p)", this);
  
  vector<const void*>::iterator it;

  for (it = __heap_addresses.begin(); it != __heap_addresses.end(); it++) {

#ifdef _DEBUG
    unsigned long itl = (unsigned long) *it;
    unsigned long ral = (unsigned long) raw_address;

    cerr << "Reference::HeapTracked::is_on_heap heap address=" << *it
	 << " diff=" << (ral - itl) << endl;
#endif

    if (*it == raw_address)
      break;
  }

  if ( it != __heap_addresses.end() ) {

    __heap_addresses.erase (it);
    __heap_state = is_on_heap;

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap true heap_state="
         << (int)__heap_state << endl;
#endif

    return true;
  }

  __heap_state = ~is_on_heap;

#ifdef _DEBUG
    cerr << "Reference::HeapTracked::is_on_heap false heap_state="
         << (int)__heap_state << endl;
#endif

  return false;
}
