#include "TextInterface.h"

#include <iostream>
using namespace std;

class tester : public Reference::Able {

public:
  tester () { value = 0; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
protected:
  double value;

};

class testerTUI : public TextInterface::ClassGetSet<tester> {

public:
  testerTUI () {
   Generator<double> generator;
   add (generator.described("value", "description", &tester::get_value));
  }

};

int main ()
{
  tester test;

  TextInterface::Allocator<tester,double> allocate;

  TextInterface::Attribute<tester>* interface;
  interface = allocate.named ("value", &tester::get_value, &tester::set_value);

  interface->set_value (&test, "3.456");

  cerr << "tester::get_value=" << test.get_value() << endl;

  if (test.get_value() != 3.456) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  TextInterface::Attribute<tester>* read_only;
  read_only = allocate.named ("value", &tester::get_value);

  cerr << "AttributeGet::get_value=" << read_only->get_value(&test) << endl;

  if (read_only->get_value(&test) != "3.456") {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  try {
    read_only->set_value (&test, "0.789");

    cerr << "AttributeGet::set_value does not raise exception" << endl;
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
    
  }
  catch (Error& err) {
    cerr << "AttributeGet::set_value raises exception as expected" << endl;
  }

  testerTUI getset;
  getset.set_instance (&test);

  cerr << "ClassGetSet::get_value=" << getset.get_value("value") << endl;

  if (getset.get_value("value") != "3.456") {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  cerr << "test_TextInterface SUCCESS!" << endl;
  return 0;
}
                  
