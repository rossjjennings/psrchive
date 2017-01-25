//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/HeapTracked.h

#ifndef __ReferenceHeapTracked_h
#define __ReferenceHeapTracked_h

#include <vector>
#include <stdlib.h>

namespace Reference {

  //! Verbosity flag
  extern bool verbose;

  //! Determines if the instance was dynamically allocated on the heap.

  /*! The HeapTracked class overloads operator new in order to store
    the addresses of instances that have been dynamically allocated on
    the heap (see the HeapTracked class and the discussion in Item 27
    of Scott Meyers, More Effective C++).

    As the number of objects constructed at any one time will
    generally be much smaller than the number of objects in existence
    at the time of deletion, it is more efficient to search the heap
    addresses during construction and to set the __heap_state
    attribute accordingly.  However, in order to handle the
    possibility of multiple inheritance, it is necessary for the
    __is_on_heap method to perform a dynamic cast of the "this"
    pointer before searching the heap addresses.  As this dynamic cast
    does not work inside the constructor of a virtually inherited
    class, it is not possible to set the __heap_state attribute during
    construction.

    Therefore, in order to improve the efficiency of the __is_on_heap
    search, it is advised to call the __is_on_heap method as soon as
    possible after construction of the object.  This is done by the
    Reference::Able::__reference method, which is called by the
    Reference::To template class when its assigment operator is
    called. 
  */
  class HeapTracked
  {
  public:

    //! Default constructor
    HeapTracked () { __heap_state = 0; }

    //! Copy constructor
    /*! Disables the compiler-generated copy of the __heap_state attribute */
    HeapTracked (const HeapTracked&) { __heap_state = 0; }

    //! Assignment operator
    /*! Disables the compiler-generated copy of the __heap_state attribute */
    HeapTracked& operator = (const HeapTracked&) { return *this; }

    //! Destructor
    /*! The dynamic_cast in __is_on_heap necessitates a virtual destructor */
    virtual ~HeapTracked () { }

    //! Records the addresses of new instances allocated on the heap
    static void* operator new (size_t size, void* ptr = 0);

    //! Placement delete overload to match the placement new overload
    static void operator delete (void* location, void* ptr);

    //! Regular delete overload
    static void operator delete (void* location);

    //! Return true if this instance is found in heap addresses
    bool __is_on_heap () const;

    //! Return the number of heap addresses awaiting processing
    static size_t get_heap_queue_size ();

  private:

    friend class Able;

    //! State of this instance
    mutable char __heap_state;

  };

}

#endif

