#include "TextInterface.h"

#include <iostream>
using namespace std;

class extension : public Reference::Able {

public:
  extension () { text = "tui import failure"; }
  void set_text (const std::string& _text) { text = _text; }
  std::string get_text () const { return text; }
protected:
  std::string text;

};

class extensionTUI : public TextInterface::ClassGetSet<extension> {

public:
  extensionTUI () {
   Generator<std::string> generator;
   add (generator.named("text", &extension::get_text, &extension::set_text));
  }

};

class tester : public Reference::Able {

public:
  tester () { value = 0; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
  extension ext;
protected:
  double value;
};


class testerTUI : public TextInterface::CompositeGetSet<tester> {

public:
  testerTUI () {
   Generator<double> generator;
   add (generator.described("value", "description", &tester::get_value));
  }

};

class glue : public TextInterface::ComponentGetSet<tester,extension>
{
public:
  glue (extensionTUI* tui)
    : TextInterface::ComponentGetSet<tester,extension> ("ext", tui) { }

  extension* extract_component (tester* t) { return &(t->ext); }
};


int main () try {

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

  cerr << "testing import" << endl;

  extensionTUI tui;
  getset.import (new glue (&tui));

  unsigned nattribute = getset.get_nattribute();

  cerr << "CompositeGetSet has " << nattribute << " attributes after import"
       << endl;

  if (nattribute != 2) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  for (unsigned i=0; i < nattribute; i++)
    cerr << "  " << getset.get_name (i) << endl;

  std::string teststring = "test of TextInterface::import passed";

  getset.set_value ("ext:text", teststring);

  cerr << test.ext.get_text() << endl;

  if (test.ext.get_text() != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  if (getset.get_value("ext:text") != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  cerr << "test_TextInterface SUCCESS!" << endl;
  return 0;
}
catch (Error& error) {
  cerr << "test_TextInterface ERROR " << error << endl;
  return -1;
}

