/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <string>

using namespace std;

// a simple command for replacing the extension on a filename
string replace_extension (const string& filename, const string& ext)
{
  if (!(ext.length() && filename.length()))
    return filename;

  unsigned index = filename.find_last_of( ".", filename.length() );
  if (index == string::npos)
    index = filename.length();

  string retval = filename.substr(0, index);

  if (ext[0] != '.')
    retval += ".";

  retval += ext;

  return retval;
}
