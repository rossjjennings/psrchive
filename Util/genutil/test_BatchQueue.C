#include "BatchQueue.h"

#include <iostream>

using namespace std;

static unsigned method_calls = 0;

class A;

A* global_a = 0;

class A : public Reference::Able {
public:
  void method ()
  {
    Reference::To<A> a = new A;
    Reference::To<A> b = global_a;
    method_calls++;
#ifdef _DEBUG
    cerr << "A::method\n"; 
#endif
  }
};

int main () try {

  BatchQueue queue;

#if HAVE_PTHREAD
  cerr << "Using 8 threads" << endl;
  queue.resize (8);
#endif

  global_a = new A;

  unsigned total = 123456;
  cerr << "Spawning " << total << " jobs" << endl;

  A a;
  for (unsigned i=0; i<total; i++)
    queue.submit (&a, &A::method);
  
  queue.wait ();

  if (method_calls != total) {
    cerr << "Error: submitted=" << total << " finished=" << method_calls
	 << endl;
    return -1;
  }

  cerr << "BatchQueue test passed" << endl;
  return 0;

}
 catch (Error& error) {
   cerr << error << endl;
   return -1;
 }
