#include "TextInterface.h"

#include <iostream>
using namespace std;

class tester {

public:
  tester () { value = 0; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
protected:
  double value;

};

int main ()
{
  tester test;

  TextInterface::Allocator<tester,double> allocate;

  TextInterface::Attribute<tester>* interface;
  interface = allocate.new_Attribute ("value",
                                      &tester::set_value, &tester::get_value);

  interface->set_value (&test, "3.456");

  cerr << "tester::get_value=" << test.get_value() << endl;

  if (test.get_value() != 3.456) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  return 0;

}
                  
