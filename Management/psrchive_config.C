/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This simple program outputs a configuration file containing all current
  configuration parameters and their default values.
*/

#include "Pulsar/psrchive.h"

#include "Pulsar/Interpreter.h"
#include "Pulsar/Config.h"

using namespace std;

string find_and_replace (string text, string replace, string with)
{
  std::string remain = text;
  std::string result;

  std::string::size_type start;

  while ( (start = remain.find(replace)) != std::string::npos ) {

    // string preceding the variable substitution
    std::string before = remain.substr (0, start);

    result += before + with;

    // remove the text preceding replace
    remain = remain.substr (start + replace.length());

  }

  return result + remain;
}

int main (int argc, char** argv)
{
  /*
    The following call ensures that all configuration symbols are linked.
  */
  standard_shell();

  Pulsar::Config::Interface* interface = Pulsar::Config::get_interface();

  // find the maximum string length of the name and description
  for (unsigned i=0; i<interface->get_nvalue(); i++) {

    string name = interface->get_name (i);

    TextInterface::Value* value = interface->find (name);

    string detail = value->get_detailed_description ();

    // insert comment symbols in front of new lines
    detail = find_and_replace (detail, "\n", "\n# ");

    string commented = "# ";

    // if the value has been configured in a file, do not comment its entry
    if ( Pulsar::Config::get_configuration()->find(name) )
      commented = "";

    cout << string(75, '#') << "\n"
      "#\n"
      "# " << value->get_name() << " - " << value->get_description() << "\n"
      "#\n"
      "# " << detail << "\n"
      "#\n" <<
      commented << value->get_name() << " = " << value->get_value() << "\n"
	 << endl;

  }

  return 0;
}
