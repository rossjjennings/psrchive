#include <iostream>

#include "Registry.h"

class A {

public:
  virtual ~A() { }

  virtual bool match (char num) = 0;

  static A* factory (char num) {
    for (unsigned ichild=0; ichild<get_nchild(); ichild++)
      if (registry[ichild]->match (num))
	return registry.create(ichild);

    cerr << "A::factory no matching child" << endl;
    return 0;
  }

  static unsigned get_nchild () { return registry.size(); }

protected:
  friend class Registry::Entry<A>;
  static Registry::List<A> registry;
  
};

Registry::List<A> A::registry;

// correct way: B has a static Registry::List<A>::Enter<B> entry;
class B : public A {
  bool match (char num) { return num == 'B'; }
  static Registry::List<A>::Enter<B> entry;  
};

Registry::List<A>::Enter<B> B::entry;

// the Registry is robust against multiple sub-class registration instances
Registry::List<A>::Enter<B> mistake; 

class C : public A {
  bool match (char num) { return num == 'C'; }
};

// the Registry entry need not be a member of the class
Registry::List<A>::Enter<C> C_entry;  


int main ()
{
  if (A::get_nchild() != 2) {
    cerr << "A::children != 2!" << endl;
    return -1;
  }

  cerr << "A::children=" << A::get_nchild() << endl;
  return 0;
}
