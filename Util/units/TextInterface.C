/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "TextInterface.h"
#include "stringtok.h"
#include "pad.h"

#include <stdio.h>
#include <string.h>

using namespace std;

bool TextInterface::label_elements = false;

string TextInterface::Class::process (const string& command)
{
  string::size_type set = command.find('=');

  // if no equals sign is present, assume that command is get key
  if (set == string::npos)
    return command + "=" + get_value (command);

  // string before the equals sign
  string before = command.substr (0,set);
  // string after the equals sign
  string after = command.substr (set+1);

  // remove any white space from the key
  string param = stringtok (before, " \t");

  set_value (param, after);
  return "";
}

string TextInterface::Class::help (bool default_value)
{
  unsigned i = 0;

  string name_label = "Attribute Name";
  string description_label = "Description";
  string value_label = (default_value) ? "Value" : "";

  unsigned max_namelen = name_label.length();
  unsigned max_descriptionlen = description_label.length();

  // find the maximum string length of the name and description
  for (i=0; i<get_nattribute(); i++) {
    if (get_name(i).length() > max_namelen)
      max_namelen = get_name(i).length();
    if (get_description(i).length() > max_descriptionlen)
      max_descriptionlen = get_description(i).length();
  }

  max_namelen += 3;
  max_descriptionlen += 3;

  // repeat the dash an appropriate number of times
  string sep (max_namelen + max_descriptionlen + value_label.length(), '-');

  // make a nice header
  string help_str =
    pad(max_namelen, name_label) + 
    pad(max_descriptionlen, description_label) + value_label + "\n" +
    sep + "\n";

  for (i=0; i<get_nattribute(); i++)
    help_str += 
      pad(max_namelen, get_name(i)) + 
      pad(max_descriptionlen, get_description(i)) +
      ((default_value) ? (get_value(get_name(i)) + "\n") : "\n");

  return help_str;
}

/*! Parses text into key, range, and remainder
  \retval true if key matches name
*/
bool TextInterface::match (const string& name, const string& text,
			   string* range, string* remainder)
{
  string::size_type length = name.length();

  if ( length && strncmp (text.c_str(), name.c_str(), length) != 0 )
    return false;

  string::size_type end = text.find (':', length);
  if (end == string::npos)
    return false;

  // the range is everything between the end of the variable name and the colon
  *range = text.substr (length, end-length);

  // the remainder is everything following the colon
  *remainder = text.substr (end+1);

  // a map may have no name
  if (!length)
    return true;

  // the range may be a wildcard
  if ((*range) == "*" || (*range) == "?")
    return true;

  // the range must be enclosed in square brackets
  length = range->length();
  if ((*range)[0] == '[' && (*range)[length-1] == ']')
    return true;

  return false;
}

void TextInterface::parse_indeces (vector<unsigned>& index,
				   const string& name)
{
  string::size_type length = name.length();

  // look for the opening and closing braces
  if (name.length() < 3 || name[0] != '[' || name[length-1] != ']')
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no range in '" + name + "'");

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces range started" << endl;
#endif

  string range = name.substr (1,length-2);

  if (range.empty())
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no range in '" + name + "'");

  for (unsigned i=0; i<range.size(); i++)
    if (!isdigit(range[i]) && range[i] != '-' && range[i] != ',')
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		   "invalid range '" + range + "'");

  string backup = range;

  while (!range.empty()) {

    string sub = stringtok (range, ",", false, false);

    if (range[0]==',')
      range.erase(0,1);

    int start, end;
    char c;

    int scanned = sscanf (sub.c_str(), "%d%c%d", &start, &c, &end);

    if (scanned == 3)  {

      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces start=" << start 
	   << " end=" << end << endl;
#endif

      if (start < 0)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "start=%d < 0", start);

      if (end < 0)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "end index=%d < 0", end);

      if (end < start)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "end=%d < start=%d", end, start);


      for (int i=start; i <= end; i++)
	index.push_back (i);

    }

    else if (scanned == 1)  {

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces index=" << start << endl;
#endif

      if (start < 0)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "index=%d < 0", start);

      index.push_back (start);

    }

    else
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		                 "invalid sub-range '" + sub + "'");
  }

}

