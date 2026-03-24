/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include <iostream>
#include <cassert>

using namespace std;

#include "Reference.h"

class Manager;

class NestedPolicy: public Reference::Able
{
public:

  NestedPolicy (Manager* manager) { my_manager = manager; }

  ~NestedPolicy () { cerr << "NestedPolicy destruct" << endl; }
  Reference::To<Manager, false> my_manager;
};

class ManageAble: public Reference::Able {

public:

  ~ManageAble () { cerr << "ManageAble destruct" << endl; }
  void set_manager(Manager* manager)
  {
    if (my_manager.is_equal_to(manager))
      return;

    my_manager = manager; 
    policy = new NestedPolicy(manager);
  }

protected:
  Reference::To<Manager, false> my_manager;
  Reference::To<NestedPolicy> policy;
};

class Manager: public Reference::Able {
 
public:

  Manager () { instance_count ++; }
  ~Manager () { cerr << "Manager destruct" << endl; instance_count --; }

  void manage (ManageAble* manage_this) 
  {
    cerr << "Manager::manage set ManageAble::my_manager" << endl;
    manage_this->set_manager (this);
    cerr << "Manager::manage my_managed.push_back" << endl;
    my_managed.push_back (manage_this);
    cerr << "Manager::manage return" << endl;
  }
  
  std::vector< Reference::To<ManageAble> > my_managed;

  static unsigned instance_count;
};

unsigned Manager::instance_count = 0;

int main (int argc, char** argv)
{
  Manager* manager_ptr = new Manager;            // refcount = 0

  cerr << "Set reference" << endl;
  Reference::To<Manager> manager = manager_ptr;  // refcount = 1

  cerr << "Manager::manage" << endl;
  manager->manage (new ManageAble);              // refcount = 2 !

  cerr << "Remove reference" << endl;
  manager = 0;                                   // refcount = 1 !!!

  if (Manager::instance_count != 0)
  {
    cerr << "The Manager instance is not deleted becuase the ManageAble\n"
      "instance is still refering to it!" << endl;
    return -1;
  }

  cerr << endl << "test_circular_reference new Manager" << endl << endl;
  manager_ptr = new Manager;
  assert(Manager::instance_count == 1);

  cerr << endl << "test_circular_reference new ManageAble" << endl << endl;
  ManageAble* managable1 = new ManageAble;

  // this weak reference should not keep the ManageAble instance alive when manager is destroyed
  Reference::To<ManageAble,false> managable2 = managable1;

  cerr << endl << "test_circular_reference Manager::manage" << endl << endl;
  manager_ptr->manage(managable1);
  manager_ptr->manage(managable2);
  manager_ptr->manage(new ManageAble);

  cerr << endl << "test_circular_reference delete ManageAble" << endl << endl;
  delete managable1;

  // the Manager instance is currently in the Reference::Bin, awaiting destruction
  assert(Manager::instance_count == 1);

  cerr << endl << "test_circular_reference test for nullptr" << endl << endl;

  // the weak reference is invalidated
  assert(!managable2);

  cerr << endl << "test_circular_reference indirectly clear the bin" << endl << endl;

  // any assignment to a Reference::To will clear the bin
  managable2 = new ManageAble;

  assert(Manager::instance_count == 0);




  cerr << endl << "test_circular_reference test copy constructor" << endl << endl;

  cerr << endl << "test_circular_reference new Manager" << endl << endl;
  manager_ptr = new Manager;
  assert(Manager::instance_count == 1);

  cerr << endl << "test_circular_reference new ManageAble" << endl << endl;
  ManageAble* managable = new ManageAble;

  cerr << endl << "test_circular_reference Manager::manage" << endl << endl;
  manager_ptr->manage(managable);

  cerr << endl << "test_circular_reference new copy-constructed ManageAble" << endl << endl;
  ManageAble* managable_copy = new ManageAble(*managable);

  cerr << endl << "test_circular_reference delete ManageAble" << endl << endl;
  delete managable;

  // any assignment to a Reference::To will clear the bin
  managable2 = new ManageAble;

  // managable_copy should not keep the Manager alive
  assert(Manager::instance_count == 0);

  cerr << endl << "test_circular_reference delete ManageAble copy" << endl << endl;

  delete managable_copy;
  assert(Manager::instance_count == 0);

  cerr << "Success!" << endl;
  return 0;
}
