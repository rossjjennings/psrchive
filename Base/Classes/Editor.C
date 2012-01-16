/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Editor.h"
#include "Pulsar/ArchiveExtension.h"

using namespace std;

Pulsar::Editor::Editor ()
{
  prefix_name = true;
  output_filename = true;
}

void standard_separation (vector<string>& list, const string& str)
{
  if (str.find (';') != string::npos)
    separate (str, list, ";");
  else
    separate (str, list, ",");
}

//! Add a comma-separated list of commands to execute
void Pulsar::Editor::add_commands (const std::string& str)
{
  standard_separation (commands, str);
}


//! Add a comma-separated list of extensions to install
void Pulsar::Editor::add_extensions (const std::string& str)
{
  standard_separation (extensions, str);
}

bool Pulsar::Editor::will_modify () const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::Editor::will_modify" << endl;

  if (extensions.size() > 0)
    return true;

  for (unsigned j = 0; j < commands.size(); j++)
  {
    if (Archive::verbose > 2)
      cerr << "Pulsar::Editor::will_modify command[" << j << "]"
	" = '" << commands[j] << "'" << endl;

    if (commands[j].find('=') != string::npos)
      return true;
  }

  return false;
}

string Pulsar::Editor::process (Archive* archive)
{
  for (unsigned i = 0; i < extensions.size(); i++)
  {
    if (Archive::verbose > 1)
      cerr << "Pulsar::Editor::process adding extension '"
	   << extensions[i] << "'" << endl;

    archive->add_extension( Archive::Extension::factory( extensions[i] ) );
  }

  Reference::To<TextInterface::Parser> interface = archive->get_interface();

  if (commands.size() == 0)
    return interface->help (true);

  // so that a space precedes each parameter processed
  interface->set_indentation (" ");
  interface->set_prefix_name (prefix_name);

  if (delimiter.length())
    interface->set_delimiter (delimiter);

  string result;

  if (output_filename)
    result = archive->get_filename();

  for (unsigned j = 0; j < commands.size(); j++)
  {
    if (Archive::verbose > 1)
      cerr << "Pulsar::Editor::process command '"
	   << commands[j] << "'" << endl;

    result += interface->process (commands[j]);
  }

  return result;
}
