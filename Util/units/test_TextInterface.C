/***************************************************************************
 *
 *   Copyright (C) 2004, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "TextInterface.h"

#include <iostream>
using namespace std;

class extension : public Reference::Able {

public:

  extension () 
  {
#ifdef _DEBUG
    cerr << "extension::this=" << this << endl;
#endif
    text = "tui import failure";
  }
  extension (const extension& ext) 
  {
#ifdef _DEBUG
    cerr << "copy extension::this=" << this << endl;
#endif
    text = ext.text;
  }
  ~extension () 
  {
#ifdef _DEBUG
    cerr << "~extension::this=" << this << endl;
#endif
  }

  void set_text (const std::string& _text) 
  {
#ifdef _DEBUG
    cerr << "extension::set_text this=" << this << " text=" << _text << endl;
#endif
    text = _text;
  }

  std::string get_text () const
  {
#ifdef _DEBUG
    cerr << "extension::get_text this=" << this << endl;
#endif
    return text;
  }

protected:
  std::string text;

};

class extensionTUI : public TextInterface::To<extension> {

public:
  extensionTUI () {
    add (&extension::get_text, &extension::set_text, "text");
  }

};

class tester : public Reference::Able {

public:
  tester () { value = 0; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
  extension* get_extension() { return &ext; }

  // test the map interface
  extension* get_map (string& text) { return &ext; }

protected:
  extension ext;
  double value;
};


class testerTUI : public TextInterface::To<tester> {

public:
  testerTUI () {
    add (&tester::get_value, "value", "description");
    add (&tester::get_value, "same",  "description");
  }

};

class child : public tester {

public:
  void set_c (int _c) { c = _c; }
  int get_c () const { return c; }
protected:
  int c;
};

class childTUI : public TextInterface::To<child> {

public:
  childTUI () {
    import( testerTUI() );
    add (&child::get_c, "c", "child attribute");
    remove ("same");
  }

};


class tester_array : public Reference::Able {

public:
  tester_array (unsigned size) : array (size) { }
  vector<tester> array;

  unsigned size () const { return array.size(); }
  tester* element (unsigned i) { return &array[i]; }
};


int main () try {

  tester Test;

  TextInterface::Allocator<tester,double> allocate;

  TextInterface::Attribute<tester>* interface;
  interface = allocate.named ("value", &tester::get_value, &tester::set_value);

  interface->set_value (&Test, "3.456");

  cerr << "tester::get_value=" << Test.get_value() << endl;

  if (Test.get_value() != 3.456) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  TextInterface::Attribute<tester>* read_only;
  read_only = allocate.named ("value", &tester::get_value);

  cerr << "AttributeGet::get_value=" << read_only->get_value(&Test) << endl;

  if (read_only->get_value(&Test) != "3.456") {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  try {
    read_only->set_value (&Test, "0.789");

    cerr << "AttributeGet::set_value does not raise exception" << endl;
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
    
  }
  catch (Error& err) {
    cerr << "AttributeGet::set_value raises exception as expected" << endl;
  }

  testerTUI getset;
  getset.set_instance (&Test);

  cerr << "TextInterface::To<>::get_value="
       << getset.get_value("value") << endl;

  if (getset.get_value("value") != "3.456") {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  cerr << "testing import" << endl;

  getset.import ( "ext", extensionTUI(), &tester::get_extension );

  unsigned nattribute = getset.get_nattribute();

  cerr << "TextInterface::To<> has " << nattribute
       << " attributes after import" << endl;

  if (nattribute != 3) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  for (unsigned i=0; i < nattribute; i++)
    cerr << "  " << getset.get_name (i) << endl;

  std::string teststring = "test of TextInterface::import Component passed";
  std::string gotstring;

  getset.set_value ("ext:text", teststring);

  cerr << Test.get_extension()->get_text() << endl;

  if (Test.get_extension()->get_text() != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  if (getset.get_value("ext:text") != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  teststring = "test of TextInterface::import Element passed";
  Test.get_extension()->set_text (teststring);

  tester_array Array (5);

  Array.array[3] = Test;

  TextInterface::To<tester_array> array_getset;
  array_getset.set_instance (&Array);
  array_getset.import ( "tester", &getset, 
			&tester_array::element,
			&tester_array::size );

  nattribute = array_getset.get_nattribute();

  cerr << "TextInterface::To<test_array> has " << nattribute 
       << " attributes after import" << endl;

  if (nattribute != 3) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  for (unsigned i=0; i < nattribute; i++)
    cerr << "  " << array_getset.get_name (i) << endl;

  gotstring = array_getset.get_value("tester[3]:ext:text");

  cerr << "get_value tester[3]:ext:text=" << gotstring<< endl;

  if (gotstring != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }


  array_getset.set_value ("tester[2]:ext:text", teststring);

  gotstring = Array.array[2].get_extension()->get_text();

  cerr << "tester_array[2].get_extension()->get_text=" << gotstring << endl;

  if (gotstring != teststring) {
    cerr << "test_TextInterface ERROR! &(tester_array[2])=" 
	 << &(Array.array[2]) << endl;
    return -1;
  }

  childTUI child_tui;
  child ch;
  child_tui.set_instance( &ch );

  cerr << child_tui.get_value ("value") << endl;

  try {
    child_tui.get_value ("same");
    cerr << "childTUI should have thrown an exception" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "childTUI successfully removed 'same' attribute" << endl;
  }

  cerr << "testing import of map interface" << endl;

  getset.import ( "map", string(), extensionTUI(), &tester::get_map );


  cerr << "test_TextInterface SUCCESS!" << endl;
  return 0;
}
catch (Error& error) {
  cerr << "test_TextInterface ERROR " << error << endl;
  return -1;
}


