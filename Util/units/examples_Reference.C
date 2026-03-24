/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

#include "Reference.h"

//! Counts total number of instances
class Counter: public Reference::Able
{
  static unsigned instance_count;
public:

  Counter () { instance_count ++; }
  Counter (const Counter&) { instance_count ++; }
  ~Counter () { instance_count --; }
  static unsigned get_instance_count() { return instance_count; }
};

unsigned Counter::instance_count = 0;

void test_set1()
{
// object on heap is deleted when reference to it is reassigned
{
  Reference::To<Counter> ref = new Counter;
  assert( Counter::get_instance_count() == 1 );

  ref = nullptr;
  assert( Counter::get_instance_count() == 0 );
}

// object on heap is deleted when reference to it goes out of scope
{
  {
    Reference::To<Counter> ref = new Counter;
    assert( Counter::get_instance_count() == 1 );
  }
  assert( Counter::get_instance_count() == 0 );
}

// object on stack is not deleted when reference to it is reassigned
{
  Counter on_stack;
  assert( Counter::get_instance_count() == 1 );

  Reference::To<Counter> ref = &on_stack;
  ref = nullptr;
  assert( Counter::get_instance_count() == 1 );
}

// object on stack is not deleted when reference to it goes out of scope
{
  Counter on_stack;
  assert( Counter::get_instance_count() == 1 );
  {
    Reference::To<Counter> ref = &on_stack;
  }
  assert( Counter::get_instance_count() == 1 );
}

// reference is invalidated when object on heap is deleted 
{
  Counter* on_heap = new Counter;
  Reference::To<Counter> ref = on_heap;

  // Reference::To<T> can be cast to bool
  assert (ref == true);

  delete on_heap;
  assert (ref == false); 
}

// reference is invalidated when object on stack goes out of scope
{
  Reference::To<Counter> ref;
  assert (ref == false); 
  {
    Counter on_stack;
    ref = &on_stack;
    assert (ref == true);
  }
  assert (ref == false);
}
}

//! successfully creates and returns a new released Counter
Counter* create ()
{
  Reference::To<Counter> ref = new Counter;

  // The release method decrements the reference count without
  // destroying the object and returns the pointer to the object
  return ref.release();
}

void test_set2()
// released object is not deleted when reference to it goes out of scope
{
  Reference::To<Counter> ref = create();
  assert( Counter::get_instance_count() == 1 );
  // call Reference::Able::get_reference_count()
  assert( ref->get_reference_count() == 1 );
}

//! throws an exception after creating a new Counter
Counter* failed_create ()
{
  Reference::To<Counter> ref = new Counter;

  // The smart pointer destroys the object when it goes out of scope
  throw Error (InvalidParam, "failed_create", "test exception handling");
}

void test_set3()
// unreleased object is deleted when reference to it goes out of scope
{
  try {
    Reference::To<Counter> ref = failed_create();
    assert (false);  // this line should not be reached
  }
  catch (Error& error)
  {
  }
  assert( Counter::get_instance_count() == 0 );
}

//! incorrectly returns pointer to Counter without releasing it
Counter* error_create ()
{
  Reference::To<Counter> ref = new Counter;
  // Reference::To<T> can be cast to T*
  return ref;
}

void test_set_fail()
// unreleased object is deleted when reference to it goes out of scope
{
  // results in a bus error or AddressSanitizer heap-use-after-free error
  Reference::To<Counter> ref = error_create();
}

class HasCounter
{
  public:
    Counter counter;
};

void test_set4()
// static member of class is not deleted when reference to it goes out of scope
{
  HasCounter* has_counter = new HasCounter;
  assert( Counter::get_instance_count() == 1 );
  {
    Reference::To<Counter> ref = &(has_counter->counter);
  }
  assert( Counter::get_instance_count() == 1 );
  assert( has_counter->counter.__is_on_heap() == false);

  delete has_counter;
  assert( Counter::get_instance_count() == 0 );
}

class Composite;

//! Part of a Composite object
class Component: public Reference::Able
{
  static unsigned instance_count;

  // reference to Composite does not add to its reference count
  Reference::To<Composite, false> composite;

public:

  Component (Composite* c) { instance_count ++; composite = c; }
  Component (const Component& that) { instance_count ++; composite = that.composite; }
  ~Component () { instance_count --; }
  bool has_composite () { return composite; }
  static unsigned get_instance_count() { return instance_count; }
};

unsigned Component::instance_count = 0;

//! Includes a Component
class Composite: public Reference::Able
{
  static unsigned instance_count;

  // reference to Composite adds to its reference count
  Reference::To<Component> component;

public:

  Composite () { instance_count ++; component = new Component(this); }
  Composite (const Composite&) { instance_count ++; component = new Component(this); }
  ~Composite () { instance_count --; }
  Component* get_component() { return component; }
  static unsigned get_instance_count() { return instance_count; }
};

unsigned Composite::instance_count = 0;

void test_set5()
{
// object with weak reference to it is deleted when strong reference to it is reassigned
{
  Reference::To<Composite> ref = new Composite;
  assert( Composite::get_instance_count() == 1 );
  assert( Component::get_instance_count() == 1 );

  // Component's reference to Composite does not contribute to Composite reference count
  assert( ref->get_reference_count() == 1 );

  ref = nullptr;
  assert( Composite::get_instance_count() == 0 );
  assert( Component::get_instance_count() == 0 );
}

// object with zero reference count is deleted when weak/passive reference to it is deleted
{
  Composite* on_heap = new Composite;
  assert( Composite::get_instance_count() == 1 );
  assert( Component::get_instance_count() == 1 );
  assert( on_heap->get_reference_count() == 0 );

  delete on_heap->get_component();

  assert( Composite::get_instance_count() == 0 );
  assert( Component::get_instance_count() == 0 );
}
}


//! Interface to a Counter object
class CounterInterface: public Reference::Able
{
  // reference to Counter does not add to its reference count
  Reference::To<Counter, false> counter;

public:
  CounterInterface (Counter* c) { counter = c; }
  bool has_counter () { return counter; }
};

// does not release strong reference when passing to weak reference
CounterInterface* incorrectly_construct_new_interface ()
{
  Reference::To<Counter> counter = new Counter;
  Reference::To<CounterInterface> interface = new CounterInterface(counter);
  assert( interface->has_counter() == true );
  return interface.release();
}

void test_set6()
// weak reference does not keep object alive when strong reference goes out of scope
{
  Reference::To<CounterInterface> interface = incorrectly_construct_new_interface();
  assert( interface->has_counter() == false );
  assert( Counter::get_instance_count() == 0 );
}

// release strong reference when passing to weak reference
CounterInterface* correctly_construct_new_interface ()
{
  Reference::To<Counter> counter = new Counter;
  Reference::To<CounterInterface> interface = new CounterInterface(counter.release());
  assert( interface->has_counter() == true );
  return interface.release();
}

void test_set7()
// weak reference keeps released object alive
{
  Reference::To<CounterInterface> interface = correctly_construct_new_interface();
  assert( interface->has_counter() == true );
  assert( Counter::get_instance_count() == 1 );
}
int main ()
{
  test_set1();
  test_set2();
  test_set3();
  // test_set_fail(); // causes bus error
  test_set4();
  test_set5();
  test_set6();
  test_set7();

  return 0;
}
