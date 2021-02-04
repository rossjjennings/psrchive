/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ExtensionInterpreter.h"
#include "Pulsar/ArchiveExtension.h"

using namespace std;

Pulsar::ExtensionInterpreter::ExtensionInterpreter ()
{
  add_command 
    ( &ExtensionInterpreter::add,
      "add", "add new extension",
      "usage: add <name> \n"
      "  string name  name of extension to add \n" );

  add_command 
    ( &ExtensionInterpreter::remove,
      "remove", "remove named extension",
      "usage: pred <file> \n"
      "  string name  name of extension to remove \n" );

  add_command 
    ( &ExtensionInterpreter::list,
      "list", "list the extensions in the archive",
      "usage: list \n");

}

Pulsar::ExtensionInterpreter::~ExtensionInterpreter ()
{
}


string Pulsar::ExtensionInterpreter::add (const string& args) try
{ 
  editor.add_extension (get(), args);
  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ExtensionInterpreter::remove (const string& args) try
{ 
  editor.remove_extension (get(), args);
  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ExtensionInterpreter::list (const string& args) try
{ 
  Pulsar::Archive* archive = get();
  unsigned next = archive->get_nextension();

  string result;
  for (unsigned i=0; i<next; i++)
  {
    if (i>0)
      result += ",";
    if (archive->get_extension(i)->get_short_name().length())
      result += archive->get_extension(i)->get_short_name();
    else
      result += archive->get_extension(i)->get_extension_name();
  }
  return response (Good, result);
}
catch (Error& error) {
  return response (error);
}


