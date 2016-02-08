/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "TextInterface.h"

#include <iostream>
using namespace std;

class extension : public Reference::Able
{
public:

  extension () 
  {
    text = "tui import failure";
  }
  extension (const extension& ext) 
  {
    text = ext.text;
  }
  ~extension () 
  {
  }

  void set_text (const std::string& _text) 
  {
    text = _text;
  }

  std::string get_text () const
  {
    return text;
  }

protected:
  std::string text;
};

class extensionTUI : public TextInterface::To<extension>
{
public:
  extensionTUI (extension* instance)
  {
    set_instance (instance);
    add (&extension::get_text, &extension::set_text, "text");
  }
};

class tester : public Reference::Able
{
public:
  tester () { value = 0; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
  TextInterface::Parser* get_extension_interface()
  { return new extensionTUI(&ext); }

protected:
  extension ext;
  double value;
};


class testerTUI : public TextInterface::To<tester>
{
public:
  testerTUI ()
  {
    add (&tester::get_value, "value", "description");
    add (&tester::get_value, "same",  "description");

    add (&tester::get_value,
	 &tester::set_value,
	 &tester::get_extension_interface,
	 "embed", "direct interface to ext");
  }
};

int main () try
{
  tester Test;
  testerTUI getset;
  getset.set_instance (&Test);

  getset.set_value("embed", "3.456");

  if (getset.get_value("embed") != "3.456")
  {
    cerr <<
      "test_TextInterface ERROR:\n"
      "value = " << getset.get_value("embed") << endl;
    return -1;
  }

  // test ElementGetSet all elements in the vector
  getset.set_value("embed:text", "hello");

  if (getset.get_value("embed:text") != "hello")
  {
    cerr <<
      "test_TextInterface ERROR:\n"
      "value = " << getset.get_value("embed:text") << endl;
    return -1;
  }

  if (getset.get_value("embed") != "3.456")
  {
    cerr <<
      "test_TextInterface ERROR:\n"
      "second get value = " << getset.get_value("embed") << endl;
    return -1;
  }

  cerr << "test_DynamicInterface SUCCESS!" << endl;
  return 0;
}
catch (Error& error) {
  cerr << "test_DynamicInterface ERROR " << error << endl;
  return -1;
}


