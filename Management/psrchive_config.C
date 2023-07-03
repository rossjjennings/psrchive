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
#include "Pulsar/Config.h"

#include <cstring>
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
  bool verbose = argc > 1 && !strcmp(argv[1], "-v");

  if (verbose)
    cerr << "psrchive_config: Config::ensure_linkage" << endl;

  Pulsar::Config::ensure_linkage ();

  if (verbose)
    cerr << "psrchive_config: Config::get_configuration" << endl;

  Pulsar::Config* configuration = Pulsar::Config::get_configuration();

  if (verbose)
    cerr << "psrchive_config: Config::get_find_count" << endl;

  if (configuration->get_find_count ())
  {
    cerr << "\n"
      "psrchive_config: lazy construction model failure \n"
      " \n"
      "Global configuration variables have been constructed \n"
      "before any operations have taken place. \n"
      " \n"
      "To debug, see " PSRCHIVE_HTTP "/manuals/config/debug.shtml \n"
         << endl;

    return -1;
  }

  if (verbose)
    cerr << "psrchive_config: Config::get_interface" << endl;

  Pulsar::Config::Interface* interface = Pulsar::Config::get_interface();

  // find the maximum string length of the name and description
  for (unsigned i=0; i<interface->get_nvalue(); i++)
  {
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
