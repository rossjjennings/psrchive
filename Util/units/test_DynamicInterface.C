/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "TextInterface.h"
#include "interface_stream.h"

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

  TextInterface::Parser* get_interface();
  static extension* factory (const std::string&)
  {
    cerr << "extension::factory" << endl;
    return new extension;
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

TextInterface::Parser* extension::get_interface()
{
  return new extensionTUI (this);
}

class tester : public Reference::Able
{
public:
  tester () { value = 0; ext = new extension; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
  TextInterface::Parser* get_extension_interface()
  {
    cerr << "tester::get_extension_interface" << endl;
    return new extensionTUI(ext);
  }

  extension* get_extension ()
  {
    return ext;
  }

  void set_extension (extension* e) { ext = e; }

protected:
  Reference::To<extension> ext;
  double value;
};

std::ostream& operator<< (std::ostream& ostr,
			  extension* e)
{
  return interface_insertion (ostr, e);
}

std::istream& operator>> (std::istream& istr,
			  extension* &e)
{
  return interface_extraction (istr, e);
}


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
#if 0
    add (&tester::get_extension,
	 &tester::set_extension,
	 &extension::get_interface,
	 "embed", "direct interface to ext");
#endif
  }
};

int main () try
{
  cerr << "main: construct tester" << endl;
  tester Test;

  cerr << "main: construct testerTUI" << endl;

  testerTUI getset;
  getset.set_instance (&Test);

  getset.help(true);

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


